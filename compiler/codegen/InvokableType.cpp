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

#include "InvokableType.h"

namespace nballerina {

InvokableType::InvokableType(std::vector<Type> paramTy, Type restTy, Type retTy)
    : paramTypes(std::move(paramTy)), returnType(std::move(retTy)), restType(std::move(restTy)) {}

InvokableType::InvokableType(std::vector<Type> paramTy, Type retTy)
    : paramTypes(std::move(paramTy)), returnType(std::move(retTy)), restType() {}

const Type &InvokableType::getReturnType() const { return returnType; }
const Type &InvokableType::getParamType(int i) const { return paramTypes[i]; }
size_t InvokableType::getParamTypeCount() const { return paramTypes.size(); }

} // namespace nballerina
