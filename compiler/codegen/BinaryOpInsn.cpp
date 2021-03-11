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

#include "BinaryOpInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include <llvm-c/Core.h>
#include <string>

using namespace std;

namespace nballerina {
BinaryOpInsn::BinaryOpInsn(Operand lhs, std::shared_ptr<BasicBlock> currentBB, Operand rhsOp1, Operand rhsOp2)
    : NonTerminatorInsn(std::move(lhs), currentBB), rhsOp1(std::move(rhsOp1)), rhsOp2(std::move(rhsOp2)), kind{} {}

void BinaryOpInsn::setInstKind(InstructionKind kind) { this->kind = kind; }

void BinaryOpInsn::translate([[maybe_unused]] LLVMModuleRef &modRef) {

    auto funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();

    string lhsName = getLhsOperand().getName();
    string lhstmpName = lhsName + "_temp";
    LLVMValueRef lhsRef = funcObj->getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef rhsOp1ref = funcObj->createTempVariable(rhsOp1);
    LLVMValueRef rhsOp2ref = funcObj->createTempVariable(rhsOp2);

    LLVMValueRef ifReturn = nullptr;
    switch (kind) {
    case INSTRUCTION_KIND_BINARY_ADD: {
        ifReturn = LLVMBuildAdd(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            LLVMBuildStore(builder, ifReturn, lhsRef);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_SUB: {
        ifReturn = LLVMBuildSub(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            LLVMBuildStore(builder, ifReturn, lhsRef);
        }

        break;
    }
    case INSTRUCTION_KIND_BINARY_MUL: {
        ifReturn = LLVMBuildMul(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            LLVMBuildStore(builder, ifReturn, lhsRef);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_DIV: {
        ifReturn = LLVMBuildUDiv(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            LLVMBuildStore(builder, ifReturn, lhsRef);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_MOD: {
        ifReturn = LLVMBuildURem(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            LLVMBuildStore(builder, ifReturn, lhsRef);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_THAN: {
        ifReturn = LLVMBuildICmp(builder, LLVMIntUGT, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            funcObj->insertBranchComparisonValue(lhsName, ifReturn);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_EQUAL: {
        ifReturn = LLVMBuildICmp(builder, LLVMIntUGE, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            funcObj->insertBranchComparisonValue(lhsName, ifReturn);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_THAN: {
        ifReturn = LLVMBuildICmp(builder, LLVMIntULT, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            funcObj->insertBranchComparisonValue(lhsName, ifReturn);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_EQUAL: {
        ifReturn = LLVMBuildICmp(builder, LLVMIntULE, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            funcObj->insertBranchComparisonValue(lhsName, ifReturn);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_EQUAL: {
        ifReturn = LLVMBuildICmp(builder, LLVMIntEQ, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            funcObj->insertBranchComparisonValue(lhsName, ifReturn);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_NOT_EQUAL: {
        ifReturn = LLVMBuildICmp(builder, LLVMIntNE, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            funcObj->insertBranchComparisonValue(lhsName, ifReturn);
        }
        break;
    }
    case INSTRUCTION_KIND_BINARY_BITWISE_XOR: {
        ifReturn = LLVMBuildXor(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
        if (ifReturn != nullptr) {
            LLVMBuildStore(builder, ifReturn, lhsRef);
        }
        break;
    }
    default:
        break;
    }
}
} // namespace nballerina
