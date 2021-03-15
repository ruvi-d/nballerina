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

#include "MapInsns.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include <iostream>

using namespace std;

namespace nballerina {

// new Map Instruction and Codegen logic are in the llvmStructure.cpp

MapStoreInsn::MapStoreInsn(Operand lhs, std::shared_ptr<BasicBlock> currentBB, Operand KOp, Operand rOp)
    : NonTerminatorInsn(std::move(lhs), currentBB), keyOp(std::move(KOp)), rhsOp(std::move(rOp)) {}

void MapStoreInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();

    // Find Variable corresponding to lhs to determine member type
    auto lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    assert(lhsVar);
    const auto &mapType = lhsVar->getType();
    TypeTag memberTypeTag = mapType.getMemberTypeTag();

    // Only handle Int type
    if (memberTypeTag != TYPE_TAG_INT) {
        std::cerr << "Non INT type maps are currently not supported" << std::endl;
        llvm_unreachable("Unknown Type");
    }

    // Codegen for Map of Int type store
    LLVMValueRef mapStoreFunc = getMapIntStoreDeclaration(modRef);
    LLVMValueRef lhsOpTempRef = funcObj.createTempVariable(getLhsOperand());
    LLVMValueRef rhsOpRef = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef keyRef = funcObj.createTempVariable(keyOp);

    LLVMValueRef argOpValueRef[] = {lhsOpTempRef, keyRef, rhsOpRef};
    LLVMBuildCall(builder, mapStoreFunc, argOpValueRef, 3, "");
}

// Declaration for map<int> type store function
LLVMValueRef MapStoreInsn::getMapIntStoreDeclaration(LLVMModuleRef &modRef) {

    LLVMValueRef mapStoreFunc = getPackageRef().getFunctionRef("map_store_int");
    if (mapStoreFunc != nullptr) {
        return mapStoreFunc;
    }
    LLVMTypeRef int32PtrType = LLVMPointerType(LLVMInt32Type(), 0);
    LLVMTypeRef charArrayPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMTypeRef memPtrType = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMTypeRef paramTypes[] = {memPtrType, charArrayPtrType, int32PtrType};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMVoidType(), paramTypes, 3, 0);
    mapStoreFunc = LLVMAddFunction(modRef, "map_store_int", funcType);
    getPackageMutableRef().addFunctionRef("map_store_int", mapStoreFunc);
    return mapStoreFunc;
}

} // namespace nballerina
