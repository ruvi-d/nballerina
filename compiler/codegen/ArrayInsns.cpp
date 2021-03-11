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

#include "ArrayInstructions.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "llvm-c/Core.h"
#include <string>

using namespace std;

namespace nballerina {

// New Array Instruction
ArrayInsn::ArrayInsn(Operand lhs, std::shared_ptr<BasicBlock> currentBB, Operand sizeOp)
    : NonTerminatorInsn(std::move(lhs), currentBB), sizeOp(std::move(sizeOp)) {}

LLVMValueRef ArrayInsn::getArrayInitDeclaration(LLVMModuleRef &modRef) {

    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef("new_int_array");
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes = LLVMInt32Type();
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt32Type(), &paramTypes, 1, 0);
    addedFuncRef = LLVMAddFunction(modRef, "new_int_array", funcType);
    getPackage()->addFunctionRef("new_int_array", addedFuncRef);
    return addedFuncRef;
}

void ArrayInsn::translate(LLVMModuleRef &modRef) {
    auto funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef localTempCarRef = funcObj->createTempVariable(sizeOp);
    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef arrayInitFunc = getArrayInitDeclaration(modRef);
    LLVMValueRef newArrayRef = LLVMBuildCall(builder, arrayInitFunc, &localTempCarRef, 1, "");

    LLVMBuildStore(builder, newArrayRef, lhsOpRef);
}

// Array Load Instruction
ArrayLoadInsn::ArrayLoadInsn(Operand lhs, std::shared_ptr<BasicBlock> currentBB, Operand KOp, Operand ROp)
    : NonTerminatorInsn(std::move(lhs), currentBB), keyOp(std::move(KOp)), rhsOp(std::move(ROp)) {}

LLVMValueRef ArrayLoadInsn::getArrayLoadDeclaration(LLVMModuleRef &modRef) {
    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef("int_array_load");
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
    LLVMTypeRef paramTypes[] = {int32PtrType, LLVMInt32Type()};
    LLVMTypeRef funcType = LLVMFunctionType(int32PtrType, paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, "int_array_load", funcType);
    getPackage()->addFunctionRef("int_array_load", addedFuncRef);
    return addedFuncRef;
}

void ArrayLoadInsn::translate(LLVMModuleRef &modRef) {
    auto funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = getArrayLoadDeclaration(modRef);

    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef keyRef = funcObj->createTempVariable(keyOp);

    LLVMValueRef sizeOpValueRef[] = {rhsOpRef, keyRef};
    LLVMValueRef valueInArrayPointer = LLVMBuildCall(builder, ArrayLoadFunc, sizeOpValueRef, 2, "");
    LLVMValueRef ArrayTempVal = LLVMBuildLoad(builder, valueInArrayPointer, "");
    LLVMBuildStore(builder, ArrayTempVal, lhsOpRef);
}

// Array Store Instruction
ArrayStoreInsn::ArrayStoreInsn(Operand lhs, std::shared_ptr<BasicBlock> currentBB, Operand KOp, Operand rOp)
    : NonTerminatorInsn(std::move(lhs), currentBB), keyOp(std::move(KOp)), rhsOp(std::move(rOp)) {}

LLVMValueRef ArrayStoreInsn::getArrayStoreDeclaration(LLVMModuleRef &modRef) {

    LLVMValueRef addedFuncRef = getPackage()->getFunctionRef("int_array_store");
    if (addedFuncRef == nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
    LLVMTypeRef paramTypes[] = {int32PtrType, LLVMInt32Type(), int32PtrType};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    addedFuncRef = LLVMAddFunction(modRef, "int_array_store", funcType);
    getPackage()->addFunctionRef("int_array_store", addedFuncRef);
    return addedFuncRef;
}

void ArrayStoreInsn::translate(LLVMModuleRef &modRef) {
    auto funcObj = getFunction();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef ArrayLoadFunc = getArrayStoreDeclaration(modRef);

    LLVMValueRef lhsOpRef = funcObj->getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMValueRef rhsOpRef = funcObj->getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef keyRef = funcObj->createTempVariable(keyOp);
    LLVMValueRef argOpValueRef[] = {lhsOpRef, keyRef, rhsOpRef};

    LLVMBuildCall(builder, ArrayLoadFunc, argOpValueRef, 3, "");
}

} // namespace nballerina
