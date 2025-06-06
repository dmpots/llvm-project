//===- TestInlining.cpp - Pass to inline calls in the test dialect --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// TODO: This pass is only necessary because the main inlining pass
// has not abstracted away the call+callee relationship. When the inlining
// interface has this support, this pass should be removed.
//
//===----------------------------------------------------------------------===//

#include "TestDialect.h"
#include "TestOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/Inliner.h"
#include "mlir/Transforms/InliningUtils.h"
#include "llvm/ADT/StringSet.h"

using namespace mlir;
using namespace test;

namespace {
struct InlinerTest
    : public PassWrapper<InlinerTest, OperationPass<func::FuncOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(InlinerTest)

  StringRef getArgument() const final { return "test-inline"; }
  StringRef getDescription() const final {
    return "Test inlining region calls";
  }

  void runOnOperation() override {
    InlinerConfig config;

    auto function = getOperation();

    // Collect each of the direct function calls within the module.
    SmallVector<func::CallIndirectOp, 16> callers;
    function.walk(
        [&](func::CallIndirectOp caller) { callers.push_back(caller); });

    // Build the inliner interface.
    InlinerInterface interface(&getContext());

    // Try to inline each of the call operations.
    for (auto caller : callers) {
      auto callee = dyn_cast_or_null<FunctionalRegionOp>(
          caller.getCallee().getDefiningOp());
      if (!callee)
        continue;

      // Inline the functional region operation, but only clone the internal
      // region if there is more than one use.
      if (failed(inlineRegion(
              interface, config.getCloneCallback(), &callee.getBody(), caller,
              caller.getArgOperands(), caller.getResults(), caller.getLoc(),
              /*shouldCloneInlinedRegion=*/!callee.getResult().hasOneUse())))
        continue;

      // If the inlining was successful then erase the call and callee if
      // possible.
      caller.erase();
      if (callee.use_empty())
        callee.erase();
    }
  }
};
} // namespace

namespace mlir {
namespace test {
void registerInliner() { PassRegistration<InlinerTest>(); }
} // namespace test
} // namespace mlir
