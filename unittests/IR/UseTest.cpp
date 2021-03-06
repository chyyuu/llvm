//===- llvm/unittest/IR/UseTest.cpp - Use unit tests ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/SourceMgr.h"
#include "gtest/gtest.h"
#include <string>

using namespace llvm;

namespace {

TEST(UseTest, sort) {
  LLVMContext C;

  const char *ModuleString = "define void @f(i32 %x) {\n"
                             "entry:\n"
                             "  %v0 = add i32 %x, 0\n"
                             "  %v2 = add i32 %x, 2\n"
                             "  %v5 = add i32 %x, 5\n"
                             "  %v1 = add i32 %x, 1\n"
                             "  %v3 = add i32 %x, 3\n"
                             "  %v7 = add i32 %x, 7\n"
                             "  %v6 = add i32 %x, 6\n"
                             "  %v4 = add i32 %x, 4\n"
                             "  ret void\n"
                             "}\n";
  SMDiagnostic Err;
  Module *M = ParseAssemblyString(ModuleString, nullptr, Err, C);
  Function *F = M->getFunction("f");
  ASSERT_TRUE(F);
  ASSERT_TRUE(F->arg_begin() != F->arg_end());
  Argument &X = *F->arg_begin();
  ASSERT_EQ("x", X.getName());

  X.sortUseList([](const Use &L, const Use &R) {
    return L.getUser()->getName() < R.getUser()->getName();
  });
  unsigned I = 0;
  for (User *U : X.users())
    EXPECT_EQ("v" + std::to_string(I++), U->getName());
  ASSERT_EQ(8u, I);

  X.sortUseList([](const Use &L, const Use &R) {
    return L.getUser()->getName() > R.getUser()->getName();
  });
  I = 0;
  for (User *U : X.users())
    EXPECT_EQ("v" + std::to_string((7 - I++)), U->getName());
  ASSERT_EQ(8u, I);
}

} // end anonymous namespace
