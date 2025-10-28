/*
CS-UY 2214
Adapted from Jeff Epstein
Starter code for E20 simulator
sim.cpp
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <cstdlib>

using namespace std;

// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 1 << 13;

/**
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char *argv[])
{
  /*
      Parse the command-line arguments
  */
  char *filename = nullptr;
  bool do_help = false;
  bool arg_error = false;
  for (int i = 1; i < argc; i++)
  {
    string arg(argv[i]);
    if (arg.rfind("-", 0) == 0)
    {
      if (arg == "-h" || arg == "--help")
        do_help = true;
      else
        arg_error = true;
    }
    else
    {
      if (filename == nullptr)
        filename = argv[i];
      else
        arg_error = true;
    }
  }
  // Display error message if appropriate
  if (arg_error || do_help || filename == nullptr)
  {
    cerr << "usage " << argv[0] << " [-h] filename" << endl
         << endl;
    cerr << "Simulate E20 machine" << endl
         << endl;
    cerr << "positional arguments:" << endl;
    cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl
         << endl;
    cerr << "optional arguments:" << endl;
    cerr << "  -h, --help  show this help message and exit" << endl;
    return 1;
  }

  // Check if file is already open
  ifstream f(filename);
  if (!f.is_open())
  {
    cerr << "Can't open file " << filename << endl;
    return 1;
  }

  // Initialized processor state
  uint16_t pc = 0;
  uint16_t regArr[NUM_REGS] = {0};
  uint16_t mem[MEM_SIZE] = {0};

  // TODO: your code here. Load f and parse using load_machine_code
  load_machine_code(f, mem);

  // TODO: your code here. Do simulation.
  sim(pc, regArr, mem);

  // TODO: your code here. print the final state of the simulator before ending, using print_state
  print_state(pc, regArr, mem, 128);

  return 0;
}

/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
void load_machine_code(ifstream &f, uint16_t mem[])
{
  regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
  size_t expectedaddr = 0;
  string line;
  while (getline(f, line))
  {
    smatch sm;
    if (!regex_match(line, sm, machine_code_re))
    {
      cerr << "Can't parse line: " << line << endl;
      exit(1);
    }
    size_t addr = stoi(sm[1], nullptr, 10);
    unsigned instr = stoi(sm[2], nullptr, 2);
    if (addr != expectedaddr)
    {
      cerr << "Memory addresses encountered out of sequence: " << addr << endl;
      exit(1);
    }
    if (addr >= MEM_SIZE)
    {
      cerr << "Program too big for memory" << endl;
      exit(1);
    }
    expectedaddr++;
    mem[addr] = instr;
  }
}

/*
    Prints the current state of the simulator, including
    the current PC, the current register values,
    and the first memquantity elements of memory.

    @param pc The final value ofx the PC
    @param regs Final value of all registers
    @param memory Final value of memory
    @param memquantity How many words of memory to dump
*/
void print_state(uint16_t pc, uint16_t regs[], const uint16_t memory[], const size_t memquantity)
{
  cout << setfill(' ');
  cout << "Final state:" << endl;
  cout << "\tpc=" << setw(5) << pc << endl;

  for (size_t reg = 0; reg < NUM_REGS; reg++)
    cout << "\t$" << reg << "=" << setw(5) << regs[reg] << endl;

  cout << setfill('0');
  bool cr = false;
  for (size_t count = 0; count < memquantity; count++)
  {
    cout << hex << setw(4) << memory[count] << " ";
    cr = true;
    if (count % 8 == 7)
    {
      cout << endl;
      cr = false;
    }
  }
  if (cr)
    cout << endl;
}

/*
    Simulates a processor, executes instructions from memory.
    Supports arithmetic, logical, jump, load/store, and immediate
    instructions. Execution affects the PC, registers, and memory.
    Halts when the PC remains unchanged after an instruction.

    @param pc The initial value of the PC
    @param regs initial value of all registers
    @param memory initial value of memory (clean slate)
 */
void sim(uint16_t &pc, uint16_t regs[], uint16_t mem[])
{
  // Set a flag for halt
  bool halt = false;

  // Run until halt
  while (!halt)
  {
    // Access memory at current PC
    uint16_t curr_ins = mem[pc & 8191];

    // Control parameters
    uint16_t opCode = curr_ins >> 13;
    uint16_t func = curr_ins & 15;

    // Registers
    uint16_t rA = (curr_ins >> 10) & 7;
    uint16_t rB = (curr_ins >> 7) & 7;
    uint16_t rC = (curr_ins >> 4) & 7;

    // Immediate Values
    uint16_t imm7 = curr_ins & 127;
    // Sign extend 7 if its negative
    if (imm7 & 64)
      imm7 |= 65408;
    // Zero extend imm13
    uint16_t imm13 = curr_ins & 0x1FFF;

    // Default increment of PC
    uint16_t new_pc = pc + 1;

    // Check if instruction 2 or 3 reg
    // Execute opcodes accordingly
    // 3 reg instruction
    if (opCode == 0b000)
    {
      // add
      if (func == 0b0000)
        regs[rC] = regs[rA] + regs[rB];
      // sub
      else if (func == 0b0001)
        regs[rC] = regs[rA] - regs[rB];
      // or
      else if (func == 0b0010)
        regs[rC] = regs[rA] | regs[rB];
      // and
      else if (func == 0b0011)
        regs[rC] = regs[rA] & regs[rB];
      // slt
      else if (func == 0b0100)
        regs[rC] = (regs[rA] < regs[rB]) ? 1 : 0;
      // jr
      else if (func == 0b1000)
        new_pc = regs[rA];
    }
    // 2 reg instruction
    else
    {
      // addi
      if (opCode == 0b001)
        regs[rB] = regs[rA] + imm7;
      // j
      else if (opCode == 0b010)
        new_pc = imm13;
      // lw
      else if (opCode == 0b100)
        regs[rB] = mem[(regs[rA] + imm7) & 8191];
      // sw
      else if (opCode == 0b101)
        mem[(regs[rA] + imm7) & 8191] = regs[rB];
      // jeq
      else if (opCode == 0b110)
        new_pc = regs[rA] == regs[rB] ? (pc + 1 + imm7) : pc + 1;
      // slti
      else if (opCode == 0b111)
        regs[rB] = regs[rA] < imm7;
      // jal
      else if (opCode == 0b011)
      {
        regs[7] = pc + 1;
        new_pc = imm13;
      }
    }

    // Check for halt
    halt = (pc & 8191) == new_pc;

    // Update PC, if halt is false
    if (!halt)
      pc = new_pc;

    // Reset Rg0
    regs[0] = 0;
  }
}