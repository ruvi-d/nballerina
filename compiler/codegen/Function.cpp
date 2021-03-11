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

#include "Function.h"
#include "BasicBlock.h"
#include "ConditionBrInsn.h"
#include "FunctionParam.h"
#include "InvocableType.h"
#include "Operand.h"
#include "Package.h"
#include "TerminatorInsn.h"
#include "Types.h"
#include "llvm-c/Core.h"

namespace nballerina {

Function::Function(Package *_parentPackage, std::string name, std::string workerName, unsigned int flags)
    : parentPackage(_parentPackage), name(std::move(name)), workerName(std::move(workerName)),
      flags(flags), returnVar{}, restParam{}, llvmBuilder(nullptr), llvmFunction(nullptr) {}

// Search basic block based on the basic block ID
std::shared_ptr<BasicBlock> Function::FindBasicBlock(const std::string &id) {
    auto bb = basicBlocksMap.find(id);
    if (bb == basicBlocksMap.end()) {
        return nullptr;
    }
    return bb->second;
}

std::string Function::getName() { return name; }
const FunctionParam &Function::getParam(int i) const { return requiredParams[i]; }
unsigned int Function::getFlags() { return flags; }
const std::optional<RestParam> &Function::getRestParam() const { return restParam; }
const std::optional<Variable> &Function::getReturnVar() const { return returnVar; }
LLVMBuilderRef Function::getLLVMBuilder() { return llvmBuilder; }
LLVMValueRef Function::getLLVMFunctionValue() { return llvmFunction; }

LLVMValueRef Function::getLLVMValueForBranchComparison(const std::string &lhsName) {
    auto branch = branchComparisonList.find(lhsName);
    if (branch == branchComparisonList.end()) {
        return nullptr;
    }
    return branch->second;
}

LLVMValueRef Function::getLLVMLocalVar(const std::string &varName) const {
    auto varIt = localVarRefs.find(varName);
    if (varIt == localVarRefs.end()) {
        return nullptr;
    }
    return varIt->second;
}

LLVMValueRef Function::createTempVariable(const Operand &operand) const {
    LLVMValueRef locVRef = getLLVMLocalOrGlobalVar(operand);
    std::string tempName = operand.getName() + "_temp";
    return LLVMBuildLoad(llvmBuilder, locVRef, tempName.c_str());
}

static bool isParamter(const Variable &locVar) {
    switch (locVar.getKind()) {
    case LOCAL_VAR_KIND:
    case TEMP_VAR_KIND:
    case RETURN_VAR_KIND:
    case GLOBAL_VAR_KIND:
    case SELF_VAR_KIND:
    case CONSTANT_VAR_KIND:
        return false;
    case ARG_VAR_KIND:
        return true;
    default:
        return false;
    }
}

LLVMTypeRef Function::getLLVMTypeOfReturnVal() {

    auto retType = returnVar->getType();

    // if main function return type is void, but user wants to return some
    // value using _bal_result (global variable from BIR), change main function
    // return type from void to global variable (_bal_result) type.
    if (isMainFunction()) {
        assert(retType.getTypeTag() == TYPE_TAG_NIL);
        auto globRetVar = parentPackage->getGlobalVariable("_bal_result");
        if (!globRetVar) {
            return LLVMVoidType();
        }
        return parentPackage->getLLVMTypeOfType(globRetVar->getType());
    }
    return parentPackage->getLLVMTypeOfType(retType);
}

void Function::insertParam(FunctionParam param) { requiredParams.push_back(std::move(param)); }
void Function::setRestParam(RestParam param) { restParam = std::move(param); }
void Function::insertLocalVar(Variable var) {
    localVars.insert(std::pair<std::string, Variable>(var.getName(), std::move(var)));
}
void Function::setReturnVar(Variable var) { returnVar = std::move(var); }
void Function::insertBasicBlock(std::shared_ptr<BasicBlock> bb) {
    if (!firstBlock)
        firstBlock = bb;
    basicBlocksMap.insert(std::pair<std::string, std::shared_ptr<BasicBlock>>(bb->getId(), bb));
}
void Function::setLLVMBuilder(LLVMBuilderRef b) { llvmBuilder = b; }
void Function::setLLVMFunctionValue(LLVMValueRef newFuncRef) { llvmFunction = newFuncRef; }
void Function::insertBranchComparisonValue(const std::string &name, LLVMValueRef compRef) {
    branchComparisonList.insert(std::pair<std::string, LLVMValueRef>(name, compRef));
}

std::optional<Variable> Function::getLocalVariable(const std::string &opName) const {
    auto varIt = localVars.find(opName);
    if (varIt == localVars.end()) {
        return std::nullopt;
    }
    return varIt->second;
}

std::optional<Variable> Function::getLocalOrGlobalVariable(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        assert(parentPackage->getGlobalVariable(op.getName()));
        return parentPackage->getGlobalVariable(op.getName());
    }
    return getLocalVariable(op.getName());
}

LLVMValueRef Function::getLLVMLocalOrGlobalVar(const Operand &op) const {
    if (op.getKind() == GLOBAL_VAR_KIND) {
        assert(parentPackage->getGlobalLLVMVar(op.getName()));
        return parentPackage->getGlobalLLVMVar(op.getName());
    }
    return getLLVMLocalVar(op.getName());
}

Package *Function::getPackage() { return parentPackage; }
size_t Function::getNumParams() { return requiredParams.size(); }

bool Function::isMainFunction() { return (name.compare(MAIN_FUNCTION_NAME) == 0); }

bool Function::isExternalFunction() { return ((flags & NATIVE) == NATIVE); }

// Patches the Terminator Insn with destination Basic Block
void Function::patchBasicBlocks() {
    for (auto &basicBlock : basicBlocksMap) {
        TerminatorInsn *terminator = basicBlock.second->getTerminatorInsnPtr();
        if ((terminator == nullptr) || !terminator->isPatched()) {
            continue;
        }
        switch (terminator->getInstKind()) {
        case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
            ConditionBrInsn *instruction = (static_cast<ConditionBrInsn *>(terminator));
            auto trueBB = FindBasicBlock(instruction->getIfThenBB()->getId());
            auto falseBB = FindBasicBlock(instruction->getElseBB()->getId());
            [[maybe_unused]] BasicBlock *danglingTrueBB = instruction->getIfThenBB();
            [[maybe_unused]] BasicBlock *danglingFalseBB = instruction->getElseBB();
            instruction->setIfThenBB(trueBB.get());
            instruction->setElseBB(falseBB.get());
            instruction->setPatched();
            break;
        }
        case INSTRUCTION_KIND_GOTO: {
            auto destBB = FindBasicBlock(terminator->getNextBB()->getId());
            [[maybe_unused]] BasicBlock *danglingBB = terminator->getNextBB();
            terminator->setNextBB(destBB.get());
            terminator->setPatched();
            break;
        }
        case INSTRUCTION_KIND_CALL: {
            auto destBB = FindBasicBlock(terminator->getNextBB()->getId());
            [[maybe_unused]] BasicBlock *danglingBB = terminator->getNextBB();
            terminator->setNextBB(destBB.get());
            break;
        }
        default:
            std::fprintf(stderr, "%s:%d Invalid Insn Kind for Instruction Patching.\n", __FILE__, __LINE__);
            break;
        }
    }
}

void Function::translate(LLVMModuleRef &modRef) {

    LLVMBasicBlockRef BbRef = LLVMAppendBasicBlock(llvmFunction, "entry");
    LLVMPositionBuilderAtEnd(llvmBuilder, BbRef);

    // iterate through all local vars.
    int paramIndex = 0;
    for (auto const &it : localVars) {
        const auto &locVar = it.second;
        LLVMTypeRef varType = parentPackage->getLLVMTypeOfType(locVar.getType());
        LLVMValueRef localVarRef = LLVMBuildAlloca(llvmBuilder, varType, (locVar.getName()).c_str());
        localVarRefs.insert({locVar.getName(), localVarRef});
        if (isParamter(locVar)) {
            LLVMValueRef parmRef = LLVMGetParam(llvmFunction, paramIndex);
            std::string paramName = requiredParams[paramIndex].getName();
            LLVMSetValueName2(parmRef, paramName.c_str(), paramName.length());
            if (parmRef != nullptr) {
                LLVMBuildStore(llvmBuilder, parmRef, localVarRef);
            }
            paramIndex++;
        }
    }

    // iterate through with each basic block in the function and create them
    for (auto &bb : basicBlocksMap) {
        LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(llvmFunction, bb.second->getId().c_str());
        bb.second->setLLVMBBRef(bbRef);
    }

    // creating branch to next basic block.
    if (firstBlock && (firstBlock->getLLVMBBRef() != nullptr)) {
        LLVMBuildBr(llvmBuilder, firstBlock->getLLVMBBRef());
    }

    // Now translate the basic blocks (essentially add the instructions in them)
    for (auto &bb : basicBlocksMap) {
        LLVMPositionBuilderAtEnd(llvmBuilder, bb.second->getLLVMBBRef());
        bb.second->translate(modRef);
    }
}

} // namespace nballerina
