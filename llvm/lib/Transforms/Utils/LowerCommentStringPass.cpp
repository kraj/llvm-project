//===-- LowerCommentStringPass.cpp - Lower Comment string metadata -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
//
// LowerCommentStringPass pass lowers module-level comment string metadata
// emitted by Clang:
//
//     !comment_string.loadtime = !{!"Copyright ..."}
//
// into concrete, translation-unit–local globals.
// This Pass is enabled only for AIX.
// For each module (translation unit), the pass performs the following:
//
//   1. Creates a null-terminated, internal constant string global
//      (`__loadtime_comment_str`) containing the copyright text in
//      `__loadtime_comment` section.
//
//   2. Marks the string in `llvm.used` so it cannot be dropped by
//      optimization or LTO.
//
//   3. Attaches `!implicit.ref` metadata referencing the string to every
//      defined function in the module. The PowerPC AIX backend recognizes
//      this metadata and emits a `.ref` directive from the function to the
//      string, creating a concrete relocation that prevents the linker from
//      discarding it (as long as the referencing symbol is kept).
//
//  Input IR:
//     !comment_string.loadtime = !{!"Copyright"}
//  Output IR:
//     @__loadtime_comment_str = internal constant [N x i8] c"Copyright\00",
//                          section "__loadtime_comment"
//     @llvm.used = appending global [1 x ptr] [ptr @__loadtime_comment_str]
//
//     define i32 @func() !implicit.ref !5 { ... }
//     !5 = !{ptr @__loadtime_comment_str}
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LowerCommentStringPass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

#define DEBUG_TYPE "lower-comment-string"

using namespace llvm;

static cl::opt<bool>
    DisableCopyrightMetadata("disable-lower-comment-string", cl::ReallyHidden,
                             cl::desc("Disable LowerCommentString pass."),
                             cl::init(false));

static bool isAIXTriple(const Module &M) {
  return Triple(M.getTargetTriple()).isOSAIX();
}

PreservedAnalyses LowerCommentStringPass::run(Module &M,
                                              ModuleAnalysisManager &AM) {
  if (DisableCopyrightMetadata || !isAIXTriple(M))
    return PreservedAnalyses::all();

  LLVMContext &Ctx = M.getContext();

  // This pass processes two types of copyright/identifying information:
  // 1. A single TU-wide copyright string from #pragma comment(copyright, "...")
  // 2. Multiple user-specified variables from -mloadtime-comment-vars=...
  //
  // Both need implicit references from every function to survive:
  // - Dead code elimination (DCE) - variables appear unused
  // - Link-time optimization (LTO) - cross-TU analysis may remove them
  // - Aggressive garbage collection - static variables with no direct uses
  //
  // Strategy: Collect all copyright globals, then create implicit references
  // from every function definition to each global. This forces the backend
  // to treat them as reachable and preserve them in the final object file.
  SmallVector<GlobalValue *, 4> CopyrightGlobals;

  // =========================================================================
  // Step 1: Process #pragma comment(copyright, "...") - at most one per TU
  // =========================================================================
  // Frontend (Clang) emits module-level metadata:
  //   !comment_string.loadtime = !{!0}
  //   !0 = !{!"Copyright text here"}
  //
  // We materialize this as a global string in the __loadtime_comment section,
  // which AIX linkers recognize and include in the object file's loadtime
  // comment area (visible via 'what' command or dump -H).
  NamedMDNode *MD = M.getNamedMetadata("comment_string.loadtime");
  if (MD && MD->getNumOperands() > 0) {
    MDNode *MdNode = MD->getOperand(0);
    if (MdNode && MdNode->getNumOperands() > 0) {
      auto *MdString = dyn_cast_or_null<MDString>(MdNode->getOperand(0));
      if (MdString && !MdString->getString().empty()) {
        StringRef Text = MdString->getString();

        // Create a null-terminated string constant in the special section
        Constant *StrInit =
            ConstantDataArray::getString(Ctx, Text, /*AddNull*/ true);
        auto *StrGV = new GlobalVariable(M, StrInit->getType(),
                                         /*isConstant*/ true,
                                         GlobalValue::InternalLinkage, StrInit,
                                         "__loadtime_comment_str");
        StrGV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
        StrGV->setAlignment(Align(1));
        // Backend recognizes this section and emits it to .loadtime_comment
        StrGV->setSection("__loadtime_comment");

        // Prevent removal by optimizer passes (but not sufficient for linker)
        appendToUsed(M, {StrGV});

        // Add to list - will get implicit refs from all functions below
        CopyrightGlobals.push_back(StrGV);
      }
    }
    // Clean up the metadata - we've consumed it
    MD->eraseFromParent();
  }

  // =========================================================================
  // Step 2: Process -mloadtime-comment-vars=sccsid,version,... (CLI flag)
  // =========================================================================
  // Frontend (Clang) marks qualifying variables with metadata:
  //   @sccsid = internal global ptr @.str, !copyright.variable !{!"sccsid"}
  //
  // These are user-defined globals (char*/char[]) that should be preserved.
  // Unlike pragma strings, these already exist in the IR - we just need to
  // ensure they survive to the object file by adding implicit references.
  for (GlobalVariable &GV : M.globals()) {
    if (GV.getMetadata("copyright.variable")) {
      CopyrightGlobals.push_back(&GV);
    }
  }

  // =========================================================================
  // Step 3: Create implicit references from every function to each global
  // =========================================================================
  // The backend interprets !implicit.ref metadata as "this function uses this
  // global, even though there's no explicit load/store in the IR". This:
  // - Prevents DCE from removing the globals
  // - Forces the linker to keep them when the function is kept
  // - Works across LTO boundaries since metadata is preserved
  //
  // Each implicit.ref node references exactly ONE global. Multiple nodes
  // can be attached to a single function (e.g., !implicit.ref !1, !implicit.ref !2)
  auto AddImplicitRef = [&](Function &F, GlobalValue *GV) {
    if (F.isDeclaration())
      return;

    // Create metadata: !N = !{ptr @global_variable}
    Metadata *Ops[] = {ConstantAsMetadata::get(GV)};
    MDNode *NewMD = MDNode::get(Ctx, Ops);

    // Attach to function - addMetadata (not setMetadata) allows multiple
    // !implicit.ref nodes per function, one for each copyright global
    F.addMetadata(LLVMContext::MD_implicit_ref, *NewMD);

    LLVM_DEBUG(dbgs() << "[copyright] attached implicit.ref to function: "
                      << F.getName() << " for global: " << GV->getName()
                      << "\n");
  };

  // Apply implicit references: for each global, mark all functions as users
  if (!CopyrightGlobals.empty()) {
    for (GlobalValue *GV : CopyrightGlobals) {
      for (Function &F : M)
        AddImplicitRef(F, GV);
    }
  }

  LLVM_DEBUG(dbgs() << "[copyright] processed " << CopyrightGlobals.size()
                    << " copyright globals\n");
  return PreservedAnalyses::all();
}
