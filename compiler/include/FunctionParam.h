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

#ifndef __FUNCTIONPARAM__H__
#define __FUNCTIONPARAM__H__

#include "Operand.h"
#include <Types.h>
#include <memory>
#include <string>

namespace nballerina {

class FunctionParam : public Operand {
  private:
    std::unique_ptr<Type> type;

  public:
    FunctionParam() = delete;
    FunctionParam(std::string name) : Operand(std::move(name), ARG_VAR_KIND), type() {}
    ~FunctionParam() = default;

    const Type &getType() const {
        assert(type);
        return *type.get();
    }
    void setType(std::unique_ptr<Type> type) { this->type = std::move(type); }
};

} // namespace nballerina

#endif //!__FUNCTIONPARAM__H__
