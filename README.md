# mdpu.c
MDPU emulator in a single file of pure C along with the official development repository.

## Table of Contents
- [mdpu.c](#mdpuc)
  - [Overview](#overview)
  - [Supported Opcodes](#supported-opcodes)
  - [Usage](#usage)
  - [License](#license)

## Overview
The MDPU (Multi-Dimensional Processing Unit) is a theoretical PU (Processing Unit) that combines the general purpose of the CPU (Central Processing Unit) and the layered system of the TPU (Tensor Processing Unit).

## Supported Opcodes
- `ADD` - Add two values
- `SUB` - Subtract two values
- `MUL` - Multiply two values
- `DIV` - Divide two values
- `STORE` - Store a value
- `LOAD` - Load a value
- `LOAD_IMMEDIATE` - Load an immediate value
- `PUSH` - Push a value onto the stack
- `POP` - Pop a value from the stack
- `JMP` - Jump to a location
- `JZ` - Jump if zero
- `JNZ` - Jump if not zero
- `MOV` - Move a value
- `JE` - Jump if equal
- `JNE` - Jump if not equal
- `AND` - Bitwise AND
- `OR` - Bitwise OR
- `XOR` - Bitwise XOR
- `NOT` - Bitwise NOT
- `SHL` - Bitwise shift left
- `SHR` - Bitwise shift right
- `CMP` - Compare two values
- `TEST` - Test two values
- `B` - Branch to a location
- `BZ` - Branch if zero
- `BNZ` - Branch if not zero
- `HALT` - Halt the program

## Usage
To compile the program, run the following command:
```sh
gcc -o mdpu mdpu.c
```

To run the MDPU emulator, run the following command:
```sh
./mdpu 4x4 10
```
This will run the MDPU emulator with 4x4 (16) registers and 10 memory cells.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
