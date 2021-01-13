# This repository contains code for translating Ballerina IR to LLVM IR

## Prerequisites (Ubuntu 20.04)
* `sudo apt install build-essential llvm-11-dev`

## Build steps
1.  `cd BIR2llvmIR`
2. `make`

## Usage
* Output binary will be at `build/nballerina/nballerinacc`
* Run nballerinacc against a BIR dump file to generate the .ll LLVM IR file
 
        ./nballerinacc foo
* The .ll file can be compiled into an a.out by invoking clang with -O0 option.
  * The -O0 is required because the optimizer will otherwise optimize everything away. 
* Running the a.out and checking the return value on the command-line by using "echo $?" will yield the int value returned by the function. 
* The a.out can be disassembled using "objdump -d" to see the machine instructions generated in main.
