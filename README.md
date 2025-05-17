Disassembler for ARM Cortex-M4 CPU

TODO:
- create loader with libelf and load .text section of binary  DONE
- disassemble with Capstone
- start making own disassembler:
- - decode opcodes
  - convert to Thumb/Thumb-2 instructions
  - add control flow
  - add symbol resolution
- write example test files:
  - for x86_64    DONE
  - for arm-linux 
  - for arm-none
