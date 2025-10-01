//===- MIR2VecTest.cpp ---------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MIR2Vec.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Triple.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace mir2vec;
using VocabMap = std::map<std::string, ir2vec::Embedding>;

namespace {

TEST(MIR2VecTest, RegexExtraction) {
  // Test simple instruction names
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("NOP"), "NOP");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("RET"), "RET");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("ADD16ri"), "ADD");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("ADD32rr"), "ADD");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("ADD64rm"), "ADD");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("MOV8ri"), "MOV");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("MOV32mr"), "MOV");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("PUSH64r"), "PUSH");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("POP64r"), "POP");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("JMP_4"), "JMP");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("CALL64pcrel32"), "CALL");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("SOME_INSTR_123"), "SOME_INSTR");
  EXPECT_EQ(Vocabulary::extractBaseOpcodeName("123ADD"), "ADD");
  EXPECT_FALSE(Vocabulary::extractBaseOpcodeName("123").empty());
}

class MIR2VecVocabTestFixture : public ::testing::Test {
protected:
  std::unique_ptr<LLVMContext> Ctx;
  std::unique_ptr<Module> M;
  std::unique_ptr<TargetMachine> TM;
  const TargetInstrInfo *TII;

  void SetUp() override {
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();

    Ctx = std::make_unique<LLVMContext>();
    M = std::make_unique<Module>("test", *Ctx);

    // Set up X86 target
    Triple TargetTriple("x86_64-unknown-linux-gnu");
    M->setTargetTriple(TargetTriple);

    std::string Error;
    const Target *TheTarget =
        TargetRegistry::lookupTarget(M->getTargetTriple(), Error);
    ASSERT_TRUE(TheTarget) << "Failed to lookup target: " << Error;

    TargetOptions Options;
    TM = std::unique_ptr<TargetMachine>(TheTarget->createTargetMachine(
        M->getTargetTriple(), "", "", Options, Reloc::Model::Static));
    ASSERT_TRUE(TM);

    // Create a dummy function to get subtarget info
    FunctionType *FT = FunctionType::get(Type::getVoidTy(*Ctx), false);
    Function *F =
        Function::Create(FT, Function::ExternalLinkage, "test", M.get());

    // Get the target instruction info
    TII = TM->getSubtargetImpl(*F)->getInstrInfo();
    ASSERT_TRUE(TII);
  }

  void TearDown() override {
    TM.reset();
    M.reset();
    Ctx.reset();
  }
};

TEST_F(MIR2VecVocabTestFixture, CanonicalOpcodeMappingTest) {
  // Test that same base opcodes get same canonical indices
  std::string baseName1 = Vocabulary::extractBaseOpcodeName("ADD16ri");
  std::string baseName2 = Vocabulary::extractBaseOpcodeName("ADD32rr");
  std::string baseName3 = Vocabulary::extractBaseOpcodeName("ADD64rm");

  EXPECT_EQ(baseName1, baseName2);
  EXPECT_EQ(baseName2, baseName3);

  // Create a vocabulary instance to test the mapping
  // Use a minimal vocabulary to trigger canonical mapping construction
  VocabMap vocabMap;
  Embedding Val = Embedding(64, 1.0f);
  vocabMap["ADD"] = Val;
  Vocabulary testVocab(std::move(vocabMap), TII);

  unsigned index1 = testVocab.getCanonicalIndexForBaseName(baseName1);
  unsigned index2 = testVocab.getCanonicalIndexForBaseName(baseName2);
  unsigned index3 = testVocab.getCanonicalIndexForBaseName(baseName3);
  EXPECT_EQ(index1, index2);
  EXPECT_EQ(index2, index3);

  // Test that different base opcodes get different canonical indices
  std::string addBase = Vocabulary::extractBaseOpcodeName("ADD32rr");
  std::string subBase = Vocabulary::extractBaseOpcodeName("SUB32rr");
  std::string movBase = Vocabulary::extractBaseOpcodeName("MOV32rr");

  unsigned addIndex = testVocab.getCanonicalIndexForBaseName(addBase);
  unsigned subIndex = testVocab.getCanonicalIndexForBaseName(subBase);
  unsigned movIndex = testVocab.getCanonicalIndexForBaseName(movBase);

  EXPECT_NE(addIndex, subIndex);
  EXPECT_NE(subIndex, movIndex);
  EXPECT_NE(addIndex, movIndex);

  // Even though we only added "ADD" to the vocab, the canonical mapping
  // should assign unique indices to all the base opcodes of the target
  EXPECT_EQ(addIndex, 10u);
  EXPECT_EQ(subIndex, 1675u);
  EXPECT_EQ(movIndex, 1006u);

  // Check that the embeddings for opcodes not in the vocab are zero vectors
  EXPECT_TRUE(testVocab[addIndex].approximatelyEquals(Val));
  EXPECT_TRUE(testVocab[subIndex].approximatelyEquals(Embedding(64, 0.0f)));
  EXPECT_TRUE(testVocab[movIndex].approximatelyEquals(Embedding(64, 0.0f)));
}

// Test deterministic mapping
TEST_F(MIR2VecVocabTestFixture, DeterministicMapping) {
  // Test that the same base name always maps to the same canonical index
  std::string baseName = "ADD";

  // Create a vocabulary instance to test deterministic mapping
  // Use a minimal vocabulary to trigger canonical mapping construction
  VocabMap vocabMap;
  vocabMap["ADD"] = Embedding(64, 1.0f);
  Vocabulary testVocab(std::move(vocabMap), TII);

  unsigned index1 = testVocab.getCanonicalIndexForBaseName(baseName);
  unsigned index2 = testVocab.getCanonicalIndexForBaseName(baseName);
  unsigned index3 = testVocab.getCanonicalIndexForBaseName(baseName);

  EXPECT_EQ(index1, index2);
  EXPECT_EQ(index2, index3);

  // Test across multiple runs
  for (int i = 0; i < 100; ++i) {
    unsigned index = testVocab.getCanonicalIndexForBaseName(baseName);
    EXPECT_EQ(index, index1);
  }
}

// Test vocabulary construction
TEST_F(MIR2VecVocabTestFixture, VocabularyConstruction) {
  // Test empty vocabulary
  Vocabulary emptyVocab;
  EXPECT_FALSE(emptyVocab.isValid());

  // Test vocabulary with embeddings via VocabMap
  VocabMap vocabMap;
  vocabMap["ADD"] = Embedding(128, 1.0f); // Dimension 128, all values 1.0
  vocabMap["SUB"] = Embedding(128, 2.0f); // Dimension 128, all values 2.0

  Vocabulary vocab(std::move(vocabMap), TII);
  EXPECT_TRUE(vocab.isValid());
  EXPECT_EQ(vocab.getDimension(), 128u);

  // Test iterator - iterates over individual embeddings
  auto it = vocab.begin();
  EXPECT_NE(it, vocab.end());

  // Check first embedding exists and has correct dimension
  EXPECT_EQ((*it).size(), 128u);

  size_t count = 0;
  for (auto it = vocab.begin(); it != vocab.end(); ++it) {
    EXPECT_EQ((*it).size(), 128u);
    ++count;
  }
  EXPECT_GT(count, 0u);
}

} // namespace