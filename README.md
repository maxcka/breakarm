# breakarm - A32 Instruction Disassembler

## Overview
**breakarm** is a lightweight ARM A32 (ARMv7-A) instruction disassembler written in C.  
It parses raw ARM binary instructions and outputs human-readable assembly.

Using `libelf`, **breakarm** loads the `.text` section of ELF binaries and disassembles only that executable code segment.  
This focused approach allows efficient and accurate analysis of ARM machine code for debugging and reverse engineering.

## Features
- Supports disassembly of ARM A32 (ARMv7-A) 32-bit instructions  
- Decodes all A32 instructions except for Advanced SIMD and Floating-Point instructions  
- Loads and disassembles only the `.text` section from ELF binaries using `libelf` 
- Outputs human-readable assembly mnemonics with detailed operand decoding  
- Designed for integration into embedded development and reverse engineering workflows  
- Lightweight and easy to build with minimal dependencies 

## Getting Started

### Prerequisites

- GCC or compatible C compiler
- Make
- **libelf development libraries** (for ELF binary loading and parsing)  
  - On Debian/Ubuntu: `sudo apt install libelf-dev`   

### Build

Navigate to the `src` directory:
```bash
cd src
```
Then build the project using `make`:
```bash
make
```

### Usage
```bash
./breakarm <ELF binary>
```

## Background

### About ARM ISAs

- **Thumb**: Used in older ARM architectures; instructions are 16-bit wide.  
- **Thumb-2**: Mix of 16-bit and 32-bit instructions, extending Thumb capabilities.  
- **A32**: ARM’s classic 32-bit instruction set with fixed 32-bit instructions.  
- **A64**: 64-bit ARM instruction set used in ARMv8 and later, with fixed 32-bit instructions.

when compiling using `arm-linux-gnueabi-gcc`:
- `-marm` option generates A32 instructions
- `-mthumb` option generates Thumb (Thumb or Thumb-2 depending on `-arch` option) instructions

**breakarm** only supports disassembly of the A32 instruction set.

## Future Work
- Fix known bugs and improve overall stability
- Write more comprehensive comments and documentation throughout the codebase
- Clean up and refactor code for better readability and maintainability
- Enhance testing coverage with more unit and integration tests
- Provide more output examples of disassembled binaries








