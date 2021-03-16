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

#include "TypeCastInsn.h"
#include "Function.h"
#include "Operand.h"
#include "Package.h"
#include "Types.h"
#include "Variable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"

using namespace llvm;

namespace nballerina {

TypeCastInsn::TypeCastInsn(const Operand &lhs, std::shared_ptr<BasicBlock> currentBB, const Operand &rhsOp)
    : NonTerminatorInsn(lhs, std::move(currentBB)), rhsOp(rhsOp) {}

void TypeCastInsn::translate(LLVMModuleRef &modRef) {
    const auto &funcObj = getFunctionRef();
    LLVMBuilderRef builder = funcObj.getLLVMBuilder();
    LLVMValueRef rhsOpRef = funcObj.getLLVMLocalOrGlobalVar(rhsOp);
    LLVMValueRef lhsOpRef = funcObj.getLLVMLocalOrGlobalVar(getLhsOperand());
    LLVMTypeRef lhsTypeRef = LLVMTypeOf(lhsOpRef);
    auto lhsVar = funcObj.getLocalOrGlobalVariable(getLhsOperand());
    assert(lhsVar.has_value());
    auto rhsVar = funcObj.getLocalOrGlobalVariable(rhsOp);
    assert(rhsVar.has_value());
    auto lhsType = lhsVar->getType().getTypeTag();
    auto rhsType = rhsVar->getType().getTypeTag();

    const char *lastTypeChar = "lastTypeIdx";
    const char *origTypeChar = "origTypeIdx";

    if (rhsType == TYPE_TAG_ANY) {
        // GEP of last type of smart pointer(original type of any variable(smart pointer))
        LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, rhsOpRef, 1, lastTypeChar);
        LLVMValueRef lastTypeLoad = LLVMBuildLoad(builder, lastTypeIdx, "");
        // sign extent of GEP to i64.
        LLVMValueRef sExt = LLVMBuildSExt(builder, lastTypeLoad, LLVMInt64Type(), "");
        // Data object of smart pointer.
        LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 2, "data");
        LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");
        LLVMValueRef strTblPtr = getPackageMutableRef().getStringBuilderTableGlobalPointer();
        LLVMValueRef strTblLoad = LLVMBuildLoad(builder, strTblPtr, "");
        LLVMValueRef gepOfStr = LLVMBuildInBoundsGEP(builder, strTblLoad, &sExt, 1, "");

        // get the mangled name of the lhs type and store it to string builder table.
        std::string_view lhsTypeName = typeStringMangleName(lhsTypeRef, lhsType);
        getPackageMutableRef().addToStrTable(lhsTypeName);
        int tempRandNum = rand() % 1000 + 1;
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum, 0);
        LLVMValueRef lhsGep = LLVMBuildInBoundsGEP(builder, strTblLoad, &constValue, 1, "");
        // call is_same_type rust function to check LHS and RHS type are same or not.
        LLVMValueRef addedIsSameTypeFunc = getIsSameTypeDeclaration(modRef, lhsGep, gepOfStr);
        LLVMValueRef paramRefs[] = {lhsGep, gepOfStr};
        LLVMValueRef sameTypeResult = LLVMBuildCall(builder, addedIsSameTypeFunc, paramRefs, 2, "call");
        // creating branch condition using is_same_type() function result.
        [[maybe_unused]] LLVMValueRef brCondition = LLVMBuildIsNotNull(builder, sameTypeResult, "");
        getPackageMutableRef().addStringOffsetRelocationEntry(lhsTypeName.data(), lhsGep);

        LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, getLhsOperand().getName().c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
        LLVMBuildStore(builder, castLoad, lhsOpRef);

    } else if (lhsType == TYPE_TAG_ANY) {

        // struct first element original type
        LLVMValueRef origTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 0, origTypeChar);
        std::string_view origTypeName = typeStringMangleName(lhsTypeRef, lhsType);
        getPackageMutableRef().addToStrTable(origTypeName);
        int tempRandNum1 = rand() % 1000 + 1;
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), tempRandNum1, 0);
        LLVMValueRef origStoreRef = LLVMBuildStore(builder, constValue, origTypeIdx);
        getPackageMutableRef().addStringOffsetRelocationEntry(origTypeName.data(), origStoreRef);
        // struct second element last type
        LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, lhsOpRef, 1, lastTypeChar);
        std::string_view lastTypeName = typeStringMangleName(LLVMTypeOf(rhsOpRef), rhsType);
        getPackageMutableRef().addToStrTable(lastTypeName);
        int tempRandNum2 = rand() % 1000 + 1;
        LLVMValueRef constValue1 = LLVMConstInt(LLVMInt32Type(), tempRandNum2, 0);
        LLVMValueRef lastStoreRef = LLVMBuildStore(builder, constValue1, lastTypeIdx);
        getPackageMutableRef().addStringOffsetRelocationEntry(lastTypeName.data(), lastStoreRef);
        // struct third element void pointer data.
        LLVMValueRef elePtr2 = LLVMBuildStructGEP(builder, lhsOpRef, 2, "data");
        LLVMValueRef bitCastRes1 = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMBuildStore(builder, bitCastRes1, elePtr2);
    } else {
        LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
}

LLVMValueRef TypeCastInsn::getIsSameTypeDeclaration(LLVMModuleRef &modRef, LLVMValueRef lhsRef, LLVMValueRef rhsRef) {
    const char *isSameTypeChar = "is_same_type";
    LLVMValueRef addedFuncRef = getPackageRef().getFunctionRef(isSameTypeChar);
    if (addedFuncRef != nullptr) {
        return addedFuncRef;
    }
    LLVMTypeRef paramTypes[] = {LLVMTypeOf(lhsRef), LLVMTypeOf(rhsRef)};
    LLVMTypeRef funcType = LLVMFunctionType(LLVMInt8Type(), paramTypes, 2, 0);
    addedFuncRef = LLVMAddFunction(modRef, isSameTypeChar, funcType);
    getPackageMutableRef().addFunctionRef(isSameTypeChar, addedFuncRef);
    return addedFuncRef;
}

std::string_view TypeCastInsn::typeStringMangleName(LLVMTypeRef valType, TypeTag typeTag) {
    switch (typeTag) {
    case TYPE_TAG_INT: {
        return "__I";
    }
    case TYPE_TAG_FLOAT: {
        return "__F";
    }
    case TYPE_TAG_CHAR_STRING: {
        return "__C";
    }
    case TYPE_TAG_STRING: {
        return "__S";
    }
    case TYPE_TAG_BOOLEAN: {
        return "__B";
    }
    case TYPE_TAG_ARRAY: {
        // TODO Need to add Size of the array.
        if (unwrap(valType)->getPointerElementType()->isIntegerTy())
            return "__AI";
        return "__A";
    }
    case TYPE_TAG_ANY: {
        return "__X";
    }
    default:
        return "";
    }
}

} // namespace nballerina
