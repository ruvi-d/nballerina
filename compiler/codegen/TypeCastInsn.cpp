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

using namespace std;
using namespace llvm;

namespace nballerina {

TypeCastInsn::TypeCastInsn(Operand lhs, BasicBlock *currentBB, Operand rhsOp, [[maybe_unused]] Type *tDecl,
                           [[maybe_unused]] bool checkTypes)
    : NonTerminatorInsn(std::move(lhs), currentBB), rhsOp(std::move(rhsOp)) {}

void TypeCastInsn::translate([[maybe_unused]] LLVMModuleRef &modRef) {
    Function *funcObj = getFunction();
    string lhsOpName = getLhsOperand().getName();
    string rhsOpName = rhsOp.getName();
    LLVMBuilderRef builder = funcObj->getLLVMBuilder();
    LLVMValueRef rhsOpRef;
    LLVMValueRef lhsOpRef;
    LLVMTypeRef lhsTypeRef;

    rhsOpRef = funcObj->getLLVMLocalVar(rhsOpName);
    lhsOpRef = funcObj->getLLVMLocalVar(lhsOpName);
    lhsTypeRef = wrap(unwrap(lhsOpRef)->getType());
    Variable *orignamVarDecl = funcObj->getLocalVariable(rhsOpName);

    if (orignamVarDecl && orignamVarDecl->getType().getTypeTag() == TYPE_TAG_ANY) {
        LLVMValueRef lastTypeIdx __attribute__((unused)) = LLVMBuildStructGEP(builder, rhsOpRef, 1, "lastTypeIdx");

        // TBD: Here, We should be checking whether this type can be cast to
        // lhsType or not by calling a runtime library function in Rust.
        // And the return value of the call should be checked.
        //  If it returns false, then should be a call to abort().

        LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 2, "data");

        LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");

        LLVMValueRef castResult = LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, lhsOpName.c_str());
        LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
        LLVMBuildStore(builder, castLoad, lhsOpRef);
    } else if (funcObj->getLocalVariable(lhsOpName)->getType().getTypeTag() == TYPE_TAG_ANY) {
        LLVMValueRef structAllocaRef = funcObj->getLLVMLocalVar(getLhsOperand().getName());
        StringTableBuilder *strTable = getPackage()->getStrTableBuilder();

        // struct first element original type
        LLVMValueRef origTypeIdx = LLVMBuildStructGEP(builder, structAllocaRef, 0, "origTypeIdx");
        Variable *origVarDecl = funcObj->getLocalVariable(lhsOpName);
        TypeTag origTypeTag = origVarDecl->getType().getTypeTag();
        auto origTypeName = Type::getNameOfType(origTypeTag);
        if (!strTable->contains(origTypeName))
            strTable->add(origTypeName);
        LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), -1, 0);
        LLVMValueRef origStoreRef = LLVMBuildStore(builder, constValue, origTypeIdx);
        getPackage()->addStringOffsetRelocationEntry(origTypeName, origStoreRef);
        // struct second element last type
        LLVMValueRef lastTypeIdx = LLVMBuildStructGEP(builder, structAllocaRef, 1, "lastTypeIdx");
        Variable *lastTypeVarDecl = funcObj->getLocalVariable(rhsOpName);
        TypeTag lastTypeTag = lastTypeVarDecl->getType().getTypeTag();
        auto lastTypeName = Type::getNameOfType(lastTypeTag);
        if (!strTable->contains(lastTypeName))
            strTable->add(lastTypeName);
        LLVMValueRef constValue1 = LLVMConstInt(LLVMInt32Type(), -2, 0);
        LLVMValueRef lastStoreRef = LLVMBuildStore(builder, constValue1, lastTypeIdx);
        getPackage()->addStringOffsetRelocationEntry(lastTypeName, lastStoreRef);

        // struct third element void pointer data.
        LLVMValueRef elePtr2 = LLVMBuildStructGEP(builder, structAllocaRef, 2, "data");
        LLVMValueRef bitCastRes1 = LLVMBuildBitCast(builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
        LLVMBuildStore(builder, bitCastRes1, elePtr2);
    } else {
        LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
}

} // namespace nballerina
