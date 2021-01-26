cmake_minimum_required(VERSION 3.2)
project(nballerina VERSION 0.0.1 LANGUAGES CXX)

if(MSVC)
    message("Support For Windows has not been tested")
elseif(CMAKE_GENERATOR STREQUAL "Xcode")
    message("Support For Xcode has not been tested")
endif()

include_directories(/usr/lib/llvm-11/include)
link_directories(/usr/lib/llvm-11/lib)

file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/BIR2llvmIR/*.cpp
)

add_executable(nballerinacc ${SOURCES})
target_link_libraries(nballerinacc PRIVATE LLVM-11)
