
Andy Wu
cw4483@nyu.edu

## State of the Work

All project requirements from proj1.pdf have been met:
  - Builds cleanly with g++ -Wall -o sim *.cpp
  - Processes one .bin file given after ./sim in one command line.
  - Processes all provided .bin files properly. 
  - Read and output using provided functions in sim-starter.cpp

## Design Rationale

1. Instructions are divided into two categories:
  - 3-register format (opcode = 0b000): add, sub, and, or, slt, jr
  - 2-register/immediate format: addi, slti, lw, sw, jeq, j, jal, halt
  - This organization prevents misusing fields.

2. Halt handling
  - Halt is encoded as {j 0}, or an unconditional jump to itself.
  - So we set halt = (pc == new_pc), and process only if halt = false.

3. Built with sim-starter.cpp to maximize readability.

## Potential Weaknesses

 - Not tested under non-UTF-8 locales.
 - Regular expression assumes standard format in .bin files.







