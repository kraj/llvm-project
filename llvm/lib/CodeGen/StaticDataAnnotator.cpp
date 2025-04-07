//===- StaticDataAnnotator - Annotate static data's section prefix --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// To reason about module-wide data hotness in a module granularity, this file
// implements a module pass StaticDataAnnotator to work coordinately with the
// StaticDataSplitter pass.
//
// The StaticDataSplitter pass is a machine function pass. It analyzes data
// hotness based on code and adds counters in StaticDataProfileInfo via its
// wrapper pass StaticDataProfileInfoWrapper.
// The StaticDataProfileInfoWrapper sits in the middle between the
// StaticDataSplitter and StaticDataAnnotator passes.
// The StaticDataAnnotator pass is a module pass. It iterates global variables
// in the module, looks up counters from StaticDataProfileInfo and sets the
// section prefix based on profiles.
//
// The three-pass structure is implemented for practical reasons, to work around
// the limitation that a module pass based on legacy pass manager cannot make
// use of MachineBlockFrequencyInfo analysis. In the future, we can consider
// porting the StaticDataSplitter pass to a module-pass using the new pass
// manager framework. That way, analysis are lazily computed as opposed to
// eagerly scheduled, and a module pass can use MachineBlockFrequencyInfo.
//===----------------------------------------------------------------------===//

#include "llvm/Analysis/ProfileSummaryInfo.h"
#include "llvm/Analysis/StaticDataProfileInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/Analysis.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <set>

#define DEBUG_TYPE "static-data-annotator"

using namespace llvm;

namespace {

struct RefGraphNode;

struct RefGraphEdge {
  RefGraphEdge(RefGraphNode *Src, RefGraphNode *Dst) : Src(Src), Dst(Dst) {}

  RefGraphNode *Src = nullptr;
  RefGraphNode *Dst = nullptr;

  operator RefGraphNode *() const { return Dst; }
};

// using GVWrapper = std::pair<GlobalVariable *, int>;

struct RefGraphNode {
  RefGraphNode(const GlobalVariable *Var, int ID) : Var(Var), VarID(ID) {}

  struct EdgeComparator {
    bool operator()(const RefGraphEdge &L, const RefGraphEdge &R) const {
      return L.Dst->VarID < R.Dst->VarID;
    }
  };

  using Edge = RefGraphEdge;
  using RefEdges = std::set<RefGraphEdge, EdgeComparator>;
  using iterator = RefEdges::iterator;
  using const_iterator = RefEdges::const_iterator;

  const GlobalVariable *Var;

  int VarID;

  RefEdges Edges;
};

// TODO: Plot dot graph.
class RefGraph {
public:
  RefGraph(const Module &M) : M(M), Root(nullptr, -1) {}

private:
  void addEdge(const GlobalVariable *Src, const GlobalVariable *Dst) {
    auto SrcNode = getOrCreateNode(Src);
    auto DstNode = getOrCreateNode(Dst);
    SrcNode->Edges.insert(RefGraphEdge(SrcNode, DstNode));
  }

  RefGraphNode *getOrCreateNode(const GlobalVariable *GV) {
    auto &Node = GVToNode[GV];
    if (Node == nullptr)
      Node = std::make_unique<RefGraphNode>(GV, GVToNode.size());
    return Node.get();
  }

  void computeGVDependencies(const Value *V,
                             SmallPtrSetImpl<const GlobalVariable *> &Deps) {
    if (auto *GV = dyn_cast<const GlobalVariable>(V)) {
      Deps.insert(GV);
    } else if (auto *CE = dyn_cast<const Constant>(V)) {
      auto [Where, Inserted] = ConstantDepsCache.try_emplace(CE);
      auto &LocalDeps = Where->second;
      if (Inserted)
        for (const User *CEUser : CE->users())
          computeGVDependencies(CEUser, LocalDeps);
      Deps.insert_range(LocalDeps);
    }
  }

  void updateGVDependencies(const GlobalVariable *GV) {
    SmallPtrSet<const GlobalVariable *, 4> Deps;
    for (auto *User : GV->users())
      computeGVDependencies(User, Deps);
    Deps.erase(GV);
    for (const GlobalVariable *Dep : Deps)
      addEdge(GV, Dep);
  }

  DenseMap<const GlobalVariable *, std::unique_ptr<RefGraphNode>> GVToNode;

  const Module &M;
  RefGraphNode Root; // A dummy root

  std::unordered_map<const Constant *, SmallPtrSet<const GlobalVariable *, 4>>
      ConstantDepsCache;
};

} // namespace

/// A module pass which iterates global variables in the module and annotates
/// their section prefixes based on profile-driven analysis.
class StaticDataAnnotator : public ModulePass {
public:
  static char ID;

  StaticDataProfileInfo *SDPI = nullptr;
  const ProfileSummaryInfo *PSI = nullptr;

  StaticDataAnnotator() : ModulePass(ID) {
    initializeStaticDataAnnotatorPass(*PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<StaticDataProfileInfoWrapperPass>();
    AU.addRequired<ProfileSummaryInfoWrapperPass>();
    AU.setPreservesAll();
    ModulePass::getAnalysisUsage(AU);
  }

  StringRef getPassName() const override { return "Static Data Annotator"; }

  bool runOnModule(Module &M) override;
};

bool StaticDataAnnotator::runOnModule(Module &M) {
  SDPI = &getAnalysis<StaticDataProfileInfoWrapperPass>()
              .getStaticDataProfileInfo();
  PSI = &getAnalysis<ProfileSummaryInfoWrapperPass>().getPSI();

  if (!PSI->hasProfileSummary())
    return false;

  bool Changed = false;
  for (auto &GV : M.globals()) {
    if (GV.isDeclarationForLinker())
      continue;

    // The implementation below assumes prior passes don't set section prefixes,
    // and specifically do 'assign' rather than 'update'. So report error if a
    // section prefix is already set.
    if (auto maybeSectionPrefix = GV.getSectionPrefix();
        maybeSectionPrefix && !maybeSectionPrefix->empty())
      llvm::report_fatal_error("Global variable " + GV.getName() +
                               " already has a section prefix " +
                               *maybeSectionPrefix);

    StringRef SectionPrefix = SDPI->getConstantSectionPrefix(&GV, PSI);
    if (SectionPrefix.empty())
      continue;

    GV.setSectionPrefix(SectionPrefix);
    Changed = true;
  }

  return Changed;
}

char StaticDataAnnotator::ID = 0;

INITIALIZE_PASS(StaticDataAnnotator, DEBUG_TYPE, "Static Data Annotator", false,
                false)

ModulePass *llvm::createStaticDataAnnotatorPass() {
  return new StaticDataAnnotator();
}
