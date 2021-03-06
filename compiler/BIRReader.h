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

#ifndef BIRREADER_H
#define BIRREADER_H

#include "ArrayInstructions.h"
#include "BasicBlock.h"
#include "BinaryOpInsn.h"
#include "ConditionBrInsn.h"
#include "ConstantLoad.h"
#include "Function.h"
#include "FunctionCallInsn.h"
#include "FunctionParam.h"
#include "GoToInsn.h"
#include "InvokableType.h"
#include "MapInsns.h"
#include "MoveInsn.h"
#include "NonTerminatorInsn.h"
#include "Operand.h"
#include "Package.h"
#include "ReturnInsn.h"
#include "StructureInsn.h"
#include "TerminatorInsn.h"
#include "TypeCastInsn.h"
#include "TypeDescInsn.h"
#include "TypeTestInsn.h"
#include "Types.h"
#include "UnaryOpInsn.h"
#include "Variable.h"
#include <fstream>

class ConstantPoolSet;
class ConstantPoolEntry;
class PackageCpInfo;
class ShapeCpInfo;
class StringCpInfo;
class BIRReader;

class BIRReader {
  private:
    std::string fileName;
    std::ifstream is;
    BIRReader() {}

    ConstantPoolSet *constantPool;
    nballerina::Variable *readGlobalVar();
    nballerina::Operand *readOperand();
    nballerina::Variable *readLocalVar();
    nballerina::TypeDescInsn *readTypeDescInsn();
    nballerina::StructureInsn *readStructureInsn();
    void readInsn(nballerina::BasicBlock *basicBlock);
    nballerina::BasicBlock *readBasicBlock(nballerina::Function *birFunction);
    void patchInsn(std::vector<nballerina::BasicBlock *> basicBlocks);
    nballerina::Function *readFunction(nballerina::Package *birPackage);
    void readModule();

    // Read bytes functions
    uint8_t readU1();
    uint8_t peekReadU1();
    uint16_t readS2be();
    uint32_t readS4be();
    uint64_t readS8be();
    static bool ignoreFunction(std::string funcName);

  public:
    nballerina::Package birPackage;
    static BIRReader reader;
    static BIRReader &getInstance() { return reader; }
    void setFileStream(std::string FileName) {
        if (fileName == FileName)
            return;
        fileName = FileName;
        if (is.is_open())
            is.close();
        is.open(fileName, std::ifstream::binary);
    }
    std::string getFileName() { return fileName; }
    void deserialize();
    void setConstantPool(ConstantPoolSet *constantPoolSet) { constantPool = constantPoolSet; }
    void patchTypesToFuncParam();
    friend class ConstantPoolEntry;
    friend class ConstantPoolSet;
    friend class PackageCpInfo;
    friend class ShapeCpInfo;
    friend class StringCpInfo;
    friend class IntCpInfo;
    friend class FloatCpInfo;
    friend class BooleanCpInfo;
    friend class ByteCpInfo;
    friend class ReadCondBrInsn;
    friend class ReadBinaryInsn;
    friend class ReadUnaryInsn;
    friend class ReadConstLoadInsn;
    friend class ReadMoveInsn;
    friend class ReadFuncCallInsn;
    friend class ReadTypeDescInsn;
    friend class ReadStructureInsn;
    friend class ReadGoToInsn;
    friend class ReadReturnInsn;
    friend class ReadTypeCastInsn;
    friend class ReadTypeTestInsn;
    friend class ReadArrayInsn;
    friend class ReadArrayStoreInsn;
    friend class ReadArrayLoadInsn;
    friend class ReadMapStoreInsn;
};

class ConstantPoolEntry {

  public:
    enum tagEnum {
        TAG_ENUM_CP_ENTRY_INTEGER = 1,
        TAG_ENUM_CP_ENTRY_FLOAT = 2,
        TAG_ENUM_CP_ENTRY_BOOLEAN = 3,
        TAG_ENUM_CP_ENTRY_STRING = 4,
        TAG_ENUM_CP_ENTRY_PACKAGE = 5,
        TAG_ENUM_CP_ENTRY_BYTE = 6,
        TAG_ENUM_CP_ENTRY_SHAPE = 7
    };
    ConstantPoolEntry() {}
    virtual void read() {}

  private:
    tagEnum tag;

  public:
    BIRReader &readerRef = BIRReader::reader;
    tagEnum getTag() { return tag; }
    void setTag(tagEnum t) { tag = t; }
};

class StringCpInfo : public ConstantPoolEntry {

  public:
    StringCpInfo();
    void read();
    ~StringCpInfo();

  private:
    std::string value;

  public:
    std::string getValue() { return value; }
    void setValue(std::string str) { value = str; }
};

class ShapeCpInfo : public ConstantPoolEntry {

  public:
    ShapeCpInfo();
    void read();
    ~ShapeCpInfo();

  private:
    uint32_t shapeLength;
    std::string value;
    nballerina::TypeTag typeTag;
    uint32_t nameIndex;
    uint32_t typeFlag;
    uint32_t typeSpecialFlag;
    uint32_t paramCount;
    uint8_t hasRestType;
    uint32_t returnTypeIndex;
    uint32_t restTypeIndex;
    std::vector<uint32_t> params;
    uint32_t constraintTypeCpIndex;

  public:
    uint32_t getShapeLength() { return shapeLength; }
    std::string getValue() { return value; }
    nballerina::TypeTag getTypeTag() { return typeTag; }
    uint32_t getNameIndex() { return nameIndex; }
    uint32_t getTypeFlag() { return typeFlag; }
    uint32_t getTypeSpecialFlag() { return typeSpecialFlag; }
    uint32_t getParamCount() { return paramCount; }
    uint8_t getRestType() { return hasRestType; }
    uint32_t getReturnTypeIndex() { return returnTypeIndex; }
    uint32_t getRestTypeIndex() { return restTypeIndex; }
    void addParam(uint32_t param) { params.push_back(param); }
    uint32_t getParam(uint32_t index) { return params[index]; }
    uint32_t getConstraintTypeCpIndex() { return constraintTypeCpIndex; }

    void setShapeLength(uint32_t s) { shapeLength = s; }
    void setValue(std::string v) { value = v; }
    void setTypeTag(nballerina::TypeTag t) { typeTag = t; }
    void setNameIndex(uint32_t n) { nameIndex = n; }
    void setTypeFlag(uint32_t t) { typeFlag = t; }
    void setTypeSpecialFlag(uint32_t t) { typeSpecialFlag = t; }
    void setParamCount(uint32_t p) { paramCount = p; }
    void setRestType(uint8_t r) { hasRestType = r; }
    void setReturnTypeIndex(uint32_t r) { returnTypeIndex = r; }
    void setRestTypeIndex(uint32_t r) { restTypeIndex = r; }
};

class PackageCpInfo : public ConstantPoolEntry {

  public:
    PackageCpInfo();
    void read();
    ~PackageCpInfo();

  private:
    uint32_t orgIndex;
    uint32_t nameIndex;
    uint32_t versionIndex;

  public:
    uint32_t getOrgIndex() { return orgIndex; }
    uint32_t getNameIndex() { return nameIndex; }
    uint32_t getVersionIndex() { return versionIndex; }
    void setOrgIndex(uint32_t org) { orgIndex = org; }
    void setNameIndex(uint32_t name) { nameIndex = name; }
    void setVersionIndex(uint32_t version) { versionIndex = version; }
};

class IntCpInfo : public ConstantPoolEntry {

  public:
    IntCpInfo();
    void read();
    ~IntCpInfo();

  private:
    uint64_t value;

  public:
    uint64_t getValue() { return value; }
    void setValue(uint64_t v) { value = v; }
};

class BooleanCpInfo : public ConstantPoolEntry {

  public:
    BooleanCpInfo();
    void read();
    ~BooleanCpInfo();

  private:
    uint8_t value;

  public:
    uint8_t getValue() { return value; }
    void setValue(uint8_t v) { value = v; }
};

class FloatCpInfo : public ConstantPoolEntry {

  public:
    FloatCpInfo();
    void read();
    ~FloatCpInfo();

  private:
    double value;

  public:
    double getValue() { return value; }
    void setValue(double v) { value = v; }
};

class ByteCpInfo : public ConstantPoolEntry {

  public:
    ByteCpInfo();
    void read();
    ~ByteCpInfo();

  private:
    uint32_t value;

  public:
    uint32_t getValue() { return value; }
    void setValue(uint32_t v) { value = v; }
};

class ConstantPoolSet {

  public:
    ConstantPoolSet() {}
    void read();
    ~ConstantPoolSet();

  private:
    std::vector<ConstantPoolEntry *> *poolEntries;

  public:
    BIRReader &readerRef = BIRReader::reader;
    ConstantPoolEntry *getEntry(int index) { return (*poolEntries)[index]; }
    std::string getStringCp(uint32_t index);
    uint32_t getIntCp(uint32_t index);
    nballerina::Type *getTypeCp(uint32_t index, bool voidToInt);
    float getFloatCp(uint32_t index);
    bool getBooleanCp(uint32_t index);
    nballerina::TypeTag getTypeTag(uint32_t index);
    nballerina::InvokableType *getInvokableType(uint32_t index);
};

class ReadInsn {
  public:
    BIRReader &readerRef = BIRReader::reader;
    ReadInsn() {}
    ~ReadInsn() {}
};

class ReadNonTerminatorInstruction : public ReadInsn {
  public:
    ReadNonTerminatorInstruction() {}
    ~ReadNonTerminatorInstruction() {}
    virtual nballerina::NonTerminatorInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) = 0;
};

class ReadTerminatorInstruction : public ReadInsn {
  public:
    ReadTerminatorInstruction() {}
    ~ReadTerminatorInstruction() {}
    virtual nballerina::TerminatorInsn *readTerminatorInsn(nballerina::BasicBlock *currentBB) = 0;
};

class ReadCondBrInsn : public ReadTerminatorInstruction {
  public:
    static ReadCondBrInsn readCondBrInsn;
    ReadCondBrInsn() {}
    ~ReadCondBrInsn() {}
    nballerina::ConditionBrInsn *readTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadFuncCallInsn : public ReadTerminatorInstruction {
  public:
    static ReadFuncCallInsn readFuncCallInsn;
    ReadFuncCallInsn() {}
    ~ReadFuncCallInsn() {}
    nballerina::FunctionCallInsn *readTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadGoToInsn : public ReadTerminatorInstruction {
  public:
    static ReadGoToInsn readGoToInsn;
    ReadGoToInsn() {}
    ~ReadGoToInsn() {}
    nballerina::GoToInsn *readTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadReturnInsn : public ReadTerminatorInstruction {
  public:
    static ReadReturnInsn readReturnInsn;
    ReadReturnInsn() {}
    ~ReadReturnInsn() {}
    nballerina::ReturnInsn *readTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadBinaryInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadBinaryInsn readBinaryInsn;
    ReadBinaryInsn() {}
    ~ReadBinaryInsn() {}
    nballerina::BinaryOpInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadUnaryInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadUnaryInsn readUnaryInsn;
    ReadUnaryInsn() {}
    ~ReadUnaryInsn() {}
    nballerina::UnaryOpInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadConstLoadInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadConstLoadInsn readConstLoadInsn;
    ReadConstLoadInsn() {}
    ~ReadConstLoadInsn() {}
    nballerina::ConstantLoadInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadMoveInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadMoveInsn readMoveInsn;
    ReadMoveInsn() {}
    ~ReadMoveInsn() {}
    nballerina::MoveInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadTypeDescInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadTypeDescInsn readTypeDescInsn;
    ReadTypeDescInsn() {}
    ~ReadTypeDescInsn() {}
    nballerina::TypeDescInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadStructureInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadStructureInsn readStructureInsn;
    ReadStructureInsn() {}
    ~ReadStructureInsn() {}
    nballerina::StructureInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadTypeCastInsn : public ReadNonTerminatorInstruction {
  public:
    static ReadTypeCastInsn readTypeCastInsn;
    ReadTypeCastInsn() {}
    ~ReadTypeCastInsn() {}
    nballerina::TypeCastInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadTypeTestInsn : public ReadNonTerminatorInstruction {
  public:
    ReadTypeTestInsn() {}
    static ReadTypeTestInsn readTypeTestInsn;
    ~ReadTypeTestInsn() {}
    nballerina::TypeTestInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadArrayInsn : public ReadNonTerminatorInstruction {
  public:
    ReadArrayInsn() {}
    static ReadArrayInsn readArrayInsn;
    ~ReadArrayInsn() {}
    nballerina::ArrayInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadArrayStoreInsn : public ReadNonTerminatorInstruction {
  public:
    ReadArrayStoreInsn() {}
    static ReadArrayStoreInsn readArrayStoreInsn;
    ~ReadArrayStoreInsn() {}
    nballerina::ArrayStoreInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadArrayLoadInsn : public ReadNonTerminatorInstruction {
  public:
    ReadArrayLoadInsn() {}
    static ReadArrayLoadInsn readArrayLoadInsn;
    ~ReadArrayLoadInsn() {}
    nballerina::ArrayLoadInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

class ReadMapStoreInsn : public ReadNonTerminatorInstruction {
  public:
    ReadMapStoreInsn() {}
    static ReadMapStoreInsn readMapStoreInsn;
    ~ReadMapStoreInsn() {}
    nballerina::MapStoreInsn *readNonTerminatorInsn(nballerina::BasicBlock *currentBB) final;
};

#endif // BIRREADER_H
