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

#ifndef __FUNCTION__H__
#define __FUNCTION__H__

#include "RestParam.h"
#include "Variable.h"
#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include "interfaces/Translatable.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;
class FunctionParam;
class InvocableType;
class Type;
class Package;

class Function : public PackageNode, public Debuggable, public Translatable {
  private:
    Package *parentPackage;
    std::string name;
    std::string workerName;
    unsigned int flags;
    std::optional<Variable> returnVar;
    std::optional<RestParam> restParam;
    LLVMBuilderRef llvmBuilder;
    LLVMValueRef llvmFunction;
    std::map<std::string, Variable *> localVars;
    std::map<std::string, BasicBlock *> basicBlocksMap;
    std::vector<BasicBlock *> basicBlocks;
    std::map<std::string, LLVMValueRef> branchComparisonList;
    std::map<std::string, LLVMValueRef> localVarRefs;
    std::vector<FunctionParam> requiredParams;
    inline static const std::string MAIN_FUNCTION_NAME = "main";
    static constexpr unsigned int PUBLIC = 1;
    static constexpr unsigned int NATIVE = PUBLIC << 1;

  public:
    Function() = delete;
    Function(Package *parentPackage, std::string name, std::string workerName, unsigned int flags);
    Function(const Function &) = delete;
    ~Function() = default;

    std::string getName();
    size_t getNumParams();
    unsigned int getFlags();
    const std::optional<RestParam> &getRestParam() const;
    const std::optional<Variable> &getReturnVar() const;
    std::vector<BasicBlock *> getBasicBlocks();
    LLVMBuilderRef getLLVMBuilder();
    LLVMValueRef getLLVMFunctionValue();
    LLVMValueRef getLLVMValueForBranchComparison(const std::string &lhsName);
    BasicBlock *FindBasicBlock(const std::string &id);
    Package *getPackage() final;
    LLVMValueRef createTempVariable(const Operand &op) const;
    LLVMValueRef getLLVMLocalVar(const std::string &varName) const;
    LLVMValueRef getLLVMLocalOrGlobalVar(const Operand &op) const;
    Variable *getLocalVariable(const std::string &opName) const;
    Variable *getLocalOrGlobalVariable(const Operand &op) const;
    LLVMTypeRef getLLVMTypeOfReturnVal();
    bool isMainFunction();
    bool isExternalFunction();
    const FunctionParam &getParam(int i) const;

    void insertParam(FunctionParam param);
    void setRestParam(RestParam param);
    void setReturnVar(Variable var);
    void insertLocalVar(Variable *var);
    void insertBasicBlock(BasicBlock *bb);
    void insertBranchComparisonValue(const std::string &lhsName, LLVMValueRef compRef);
    void setLLVMBuilder(LLVMBuilderRef builder);
    void setLLVMFunctionValue(LLVMValueRef funcRef);

    void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__FUNCTION__H__
