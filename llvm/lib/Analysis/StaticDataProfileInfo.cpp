#include "llvm/Analysis/StaticDataProfileInfo.h"
#include "llvm/Analysis/ProfileSummaryInfo.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/InitializePasses.h"
#include "llvm/ProfileData/InstrProf.h"

using namespace llvm;

extern cl::opt<bool> AnnotateStaticDataSectionPrefix;

void StaticDataProfileInfo::addConstantProfileCount(
    const Constant *C, std::optional<uint64_t> Count) {
  if (!Count) {
    ConstantWithoutCounts.insert(C);
    return;
  }
  uint64_t &OriginalCount = ConstantProfileCounts[C];
  OriginalCount = llvm::SaturatingAdd(*Count, OriginalCount);
  // Clamp the count to getInstrMaxCountValue. InstrFDO reserves a few
  // large values for special use.
  if (OriginalCount > getInstrMaxCountValue())
    OriginalCount = getInstrMaxCountValue();
}

std::optional<uint64_t>
StaticDataProfileInfo::getConstantProfileCount(const Constant *C) const {
  auto I = ConstantProfileCounts.find(C);
  if (I == ConstantProfileCounts.end())
    return std::nullopt;
  return I->second;
}

std::optional<StringRef>
StaticDataProfileInfo::getSectionPrefixUsingProfileCount(
    const Constant *C, const ProfileSummaryInfo *PSI) const {
  auto Count = getConstantProfileCount(C);
  // The constant `C` doesn't have a profile count. `C` might be a external
  // linkage global variable, whose PGO-based counter is not tracked within one
  // IR module.
  if (!Count)
    return std::nullopt;
  // The accummulated counter shows the constant is hot. Return 'hot' whether
  // this variable is seen by unprofiled functions or not.
  if (PSI->isHotCount(*Count))
    return "hot";
  // The constant is not hot, and seen by unprofiled functions. We don't want to
  // assign it to unlikely sections, even if the counter says 'cold'. So return
  // an empty prefix before checking whether the counter is cold.
  if (ConstantWithoutCounts.count(C))
    return "";
  // The accummulated counter shows the constant is cold. Return 'unlikely'.
  if (PSI->isColdCount(*Count))
    return "unlikely";

  return "";
}

static StringRef reconcileHotness(StringRef PrefixFromDataAccessProf,
                                  StringRef PrefixFromAggBlockCounter) {
  assert((PrefixFromDataAccessProf == "hot" ||
          PrefixFromDataAccessProf == "unlikely") &&
         "Section prefix must be 'hot' or 'unlikely'");

  if (PrefixFromDataAccessProf == "hot" || PrefixFromAggBlockCounter == "hot")
    return "hot";
  assert(PrefixFromDataAccessProf == "unlikely" &&
         "Section prefix must be 'unlikely'.");
  return PrefixFromAggBlockCounter;
}

static StringRef
reconcileOptionalHotness(std::optional<StringRef> PrefixFromDataAccessProf,
                         std::optional<StringRef> PrefixFromAggBlockCounter) {
  if (!PrefixFromDataAccessProf) {
    if (PrefixFromAggBlockCounter && *PrefixFromAggBlockCounter == "hot") {
      return "hot";
    }
    return "";
  }
  if (!PrefixFromAggBlockCounter)
    return *PrefixFromDataAccessProf;

  return reconcileHotness(*PrefixFromDataAccessProf,
                          *PrefixFromAggBlockCounter);
}

StringRef StaticDataProfileInfo::getConstantSectionPrefix(
    const Constant *C, const ProfileSummaryInfo *PSI) const {
  std::optional<StringRef> SectionPrefixFromCounter =
      getSectionPrefixUsingProfileCount(C, PSI);
  errs() << "HasDataAccessProf: " << HasDataAccessProf << "\n";
  if (!HasDataAccessProf)
    return SectionPrefixFromCounter.value_or("");

  // If the constant `C` is a non string-literal global variable, reconcile its
  // section prefix from data-access-profile based section prefix and its
  // PGO-counter based section prefix.
  if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(C);
      GV && !GV->getName().starts_with(".str"))
    return reconcileOptionalHotness(GV->getSectionPrefix(),
                                    SectionPrefixFromCounter);

  return SectionPrefixFromCounter.value_or("");
}

bool StaticDataProfileInfoWrapperPass::doInitialization(Module &M) {
  bool HasDataAccessProf = false;
  if (auto *MD = mdconst::extract_or_null<ConstantInt>(
          M.getModuleFlag("HasDataAccessProf")))
    HasDataAccessProf = MD->getZExtValue();
  Info.reset(new StaticDataProfileInfo(HasDataAccessProf));
  return false;
}

bool StaticDataProfileInfoWrapperPass::doFinalization(Module &M) {
  Info.reset();
  return false;
}

INITIALIZE_PASS(StaticDataProfileInfoWrapperPass, "static-data-profile-info",
                "Static Data Profile Info", false, true)

StaticDataProfileInfoWrapperPass::StaticDataProfileInfoWrapperPass()
    : ImmutablePass(ID) {}

char StaticDataProfileInfoWrapperPass::ID = 0;
