# breakarm - A32 Instruction Disassembler

## Overview

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
  - On Debian/Ubuntu: `apt install libelf-dev`   

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

- Thumb - used in older architectures, 16-bit instructions
- Thumb-2 - mix of 16-bit and 32-bit instructions
- A32 - only 32-bit instructions
- A64 - only 32-bit instructions

when compiling using arm-linux-gnueabi-gcc:
- -marm option generates A32 instructions
- -mthumb option generates Thumb (Thumb or Thumb-2 depending on -arch option) instructions






