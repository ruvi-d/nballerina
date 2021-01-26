cmake_minimum_required(VERSION 3.0)
project(nballerina)

include_directories(/usr/lib/llvm-11/include)
link_directories(/usr/lib/llvm-11/lib)

file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/BIR2llvmIR/*.h
    ${PROJECT_SOURCE_DIR}/BIR2llvmIR/*.cpp
)

add_executable(nballerinacc ${SOURCES})
target_link_libraries(nballerinacc LLVM-11)
