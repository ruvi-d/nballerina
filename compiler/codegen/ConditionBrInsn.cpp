/*
 * Copyright (c) 2021, WSO2 Inc. (http://www.wso2.org) All Rights Reserved.
 *
 * WSO2 Inc. licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "ConditionBrInsn.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Operand.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"

using namespace std;

namespace nballerina {

ConditionBrInsn::ConditionBrInsn(Operand lhs, std::shared_ptr<BasicBlock> currentBB,
                                 std::shared_ptr<BasicBlock> ifThenBB, std::shared_ptr<BasicBlock> elseBB)
    : TerminatorInsn(std::move(lhs), currentBB, nullptr, true), ifThenBB(ifThenBB), elseBB(elseBB) {
    kind = INSTRUCTION_KIND_CONDITIONAL_BRANCH;
}

std::shared_ptr<BasicBlock> ConditionBrInsn::getIfThenBB() { return ifThenBB; }
std::shared_ptr<BasicBlock> ConditionBrInsn::getElseBB() { return elseBB; }
void ConditionBrInsn::setIfThenBB(std::shared_ptr<BasicBlock> bb) { ifThenBB = bb; }
void ConditionBrInsn::setElseBB(std::shared_ptr<BasicBlock> bb) { elseBB = bb; }

void ConditionBrInsn::translate(LLVMModuleRef &) {

    LLVMBuilderRef builder = getFunctionRef()->getLLVMBuilder();
    string lhsName = getLhsOperand().getName();

    LLVMValueRef brCondition = getFunctionRef()->getLLVMValueForBranchComparison(lhsName);
    if (brCondition == nullptr) {
        brCondition = LLVMBuildIsNotNull(builder, getFunctionRef()->createTempVariable(getLhsOperand()), lhsName.c_str());
    }

    LLVMBuildCondBr(builder, brCondition, ifThenBB->getLLVMBBRef(), elseBB->getLLVMBBRef());
}

} // namespace nballerina
