/*
CS-UY 2214
Adapted from Jeff Epstein
Starter code for E20 cache Simulator
simcache.cpp
*/

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <regex>
#include <string>
#include <vector>

using namespace std;

// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 1 << 13;

/*
    Prints out the correctly-formatted configuration of a cache.

    @param cache_name The name of the cache. "L1" or "L2"

    @param size The total size of the cache, measured in memory cells.
        Excludes metadata

    @param assoc The associativity of the cache. One of [1,2,4,8,16]

    @param blocksize The blocksize of the cache. One of [1,2,4,8,16,32,64])

    @param num_rows The number of rows in the given cache.
*/
void print_cache_config(const string &cache_name, int size, int assoc,
                        int blocksize, int num_rows) {
  cout << "Cache " << cache_name << " has size " << size << ", associativity "
       << assoc << ", blocksize " << blocksize << ", rows " << num_rows << endl;
}

/*
    Prints out a correctly-formatted log entry.

    @param cache_name The name of the cache where the event
        occurred. "L1" or "L2"

    @param status The kind of cache event. "SW", "HIT", or
        "MISS"

    @param pc The program counter of the memory
        access instruction

    @param addr The memory address being accessed.

    @param row The cache row or set number where the data
        is stored.
*/
void print_log_entry(const string &cache_name, const string &status, int pc,
                     int addr, int row) {
  cout << left << setw(8) << cache_name + " " + status << right
       << " pc:" << setw(5) << pc << "\taddr:" << setw(5) << addr
       << "\trow:" << setw(4) << row << endl;
}

/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
void load_machine_code(ifstream &f, uint16_t mem[]) {
  regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
  size_t expectedaddr = 0;
  string line;
  while (getline(f, line)) {
    smatch sm;
    if (!regex_match(line, sm, machine_code_re)) {
      cerr << "Can't parse line: " << line << endl;
      exit(1);
    }
    size_t addr = stoi(sm[1], nullptr, 10);
    unsigned instr = stoi(sm[2], nullptr, 2);
    if (addr != expectedaddr) {
      cerr << "Memory addresses encountered out of sequence: " << addr << endl;
      exit(1);
    }
    if (addr >= MEM_SIZE) {
      cerr << "Program too big for memory" << endl;
      exit(1);
    }
    expectedaddr++;
    mem[addr] = instr;
  }
}

// Cache data structure
struct Cache {
  bool enabled;
  string name;
  int size;
  int assoc;
  int blocksize;
  int num_rows;
  vector<vector<int>> rows;
};

// Initialize cache data structure
void init_cache(Cache &c, const string &name, int size, int assoc,
                int blocksize) {
  c.enabled = true;
  c.name = name;
  c.size = size;
  c.assoc = assoc;
  c.blocksize = blocksize;

  if (size <= 0 || assoc <= 0 || blocksize <= 0) {
    cerr << "Invalid cache parameters" << endl;
    exit(1);
  }

  c.num_rows = size / (assoc * blocksize);
  if (c.num_rows <= 0) {
    cerr << "Invalid cache configuration: num_rows <= 0" << endl;
    exit(1);
  }

  vector<int> block(assoc, -1);
  c.rows = vector<vector<int>>(c.num_rows, block);

  print_cache_config(c.name, c.size, c.assoc, c.blocksize, c.num_rows);
}

// Return whether cache is enabled
bool cache_enabled(const Cache &c) { return c.enabled; }

// Find tag index in a set, return -1 if not found
int cache_find_tag(const vector<int> &set, int tag) {
  for (int i = 0; i < (int)set.size(); i++) {
    if (set[i] == tag)
      return i;
  }
  return -1;
}

// Move element at idx to front (MRU) in set (LRU bookkeeping)
void cache_move_to_front(vector<int> &set, int idx) {
  if (idx <= 0)
    return;
  int val = set[idx];
  for (int i = idx; i > 0; i--)
    set[i] = set[i - 1];
  set[0] = val;
}

// Insert tag into set using LRU replacement
void cache_insert_tag(vector<int> &set, int tag) {
  int n = (int)set.size();
  int victim_idx = n - 1;

  // Prefer an empty (-1) slot if available
  for (int i = n - 1; i >= 0; i--) {
    if (set[i] == -1) {
      victim_idx = i;
      break;
    }
  }
  for (int i = victim_idx; i > 0; i--)
    set[i] = set[i - 1];
  set[0] = tag;
}

// L1 load: check-only, log HIT/MISS, no fill
bool cache_load_check_only(Cache &c, int addr, uint16_t pc) {
  if (!cache_enabled(c))
    return false;

  int block_id = addr / c.blocksize;
  int row_idx = block_id % c.num_rows;
  int tag = block_id / c.num_rows;

  vector<int> &set = c.rows[row_idx];

  int hit_idx = cache_find_tag(set, tag);
  if (hit_idx != -1) {
    cache_move_to_front(set, hit_idx);
    print_log_entry(c.name, "HIT", pc, addr, row_idx);
    return true;
  } else {
    print_log_entry(c.name, "MISS", pc, addr, row_idx);
    return false;
  }
}

// L2 load: log HIT/MISS, fill on MISS
bool cache_load_with_fill(Cache &c, int addr, uint16_t pc) {
  if (!cache_enabled(c))
    return false;

  int block_id = addr / c.blocksize;
  int row_idx = block_id % c.num_rows;
  int tag = block_id / c.num_rows;

  vector<int> &set = c.rows[row_idx];

  int hit_idx = cache_find_tag(set, tag);
  if (hit_idx != -1) {
    cache_move_to_front(set, hit_idx);
    print_log_entry(c.name, "HIT", pc, addr, row_idx);
    return true;
  } else {
    print_log_entry(c.name, "MISS", pc, addr, row_idx);
    cache_insert_tag(set, tag);
    return false;
  }
}

// Fill block into cache (no logging)
void cache_fill_block(Cache &c, int addr) {
  if (!cache_enabled(c))
    return;

  int block_id = addr / c.blocksize;
  int row_idx = block_id % c.num_rows;
  int tag = block_id / c.num_rows;

  vector<int> &set = c.rows[row_idx];
  int hit_idx = cache_find_tag(set, tag);
  if (hit_idx != -1)
    cache_move_to_front(set, hit_idx);
  else
    cache_insert_tag(set, tag);
}

// Store access: write-allocate + write-through, log SW
void cache_store_access(Cache &c, int addr, uint16_t pc) {
  if (!cache_enabled(c))
    return;

  int block_id = addr / c.blocksize;
  int row_idx = block_id % c.num_rows;
  int tag = block_id / c.num_rows;

  vector<int> &set = c.rows[row_idx];

  int hit_idx = cache_find_tag(set, tag);
  if (hit_idx != -1)
    cache_move_to_front(set, hit_idx);
  else
    cache_insert_tag(set, tag);

  print_log_entry(c.name, "SW", pc, addr, row_idx);
}

/*
    Simulates a processor, executes instructions from memory.
    Supports arithmetic, logical, jump, load/store, and immediate
    instructions. Execution affects the PC, registers, and memory.
    Halts when the PC remains unchanged after an instruction.

    This version also logs cache activity for lw and sw using L1 and L2.
 */
void sim(uint16_t &pc, uint16_t regs[], uint16_t mem[], Cache &L1, Cache *L2) {
  // Set a flag for halt
  bool halt = false;

  // Run until halt
  while (!halt) {
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
    if (opCode == 0b000) {
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
    else {
      // addi
      if (opCode == 0b001)
        regs[rB] = regs[rA] + imm7;
      // j
      else if (opCode == 0b010)
        new_pc = imm13;
      // lw
      else if (opCode == 0b100) {
        uint16_t addr = (regs[rA] + imm7) & 8191;

        // L1 access: check-only, log HIT/MISS
        bool hitL1 = cache_load_check_only(L1, addr, pc);

        // On L1 miss, consult L2 if present, then fill L1
        if (!hitL1) {
          if (L2 != nullptr && cache_enabled(*L2)) {
            // L2 logs and fills itself
            cache_load_with_fill(*L2, addr, pc);
          }
          // In all miss cases, block ends up in L1
          cache_fill_block(L1, addr);
        }

        regs[rB] = mem[addr];
      }
      // sw
      else if (opCode == 0b101) {
        uint16_t addr = (regs[rA] + imm7) & 8191;

        // Write-through with write-allocate:
        // always update caches, always log both caches (if L2 exists)
        cache_store_access(L1, addr, pc);
        if (L2 != nullptr && cache_enabled(*L2))
          cache_store_access(*L2, addr, pc);

        mem[addr] = regs[rB];
      }
      // jeq
      else if (opCode == 0b110)
        new_pc = regs[rA] == regs[rB] ? (pc + 1 + imm7) : pc + 1;
      // slti
      else if (opCode == 0b111)
        regs[rB] = regs[rA] < imm7;
      // jal
      else if (opCode == 0b011) {
        regs[7] = pc + 1;
        new_pc = imm13;
      }
    }

    // Check for halt
    halt = (pc == new_pc);

    // Update PC, if halt is false
    if (!halt)
      pc = new_pc;

    // Reset Rg0
    regs[0] = 0;
  }
}

/**
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char *argv[]) {
  /*
      Parse the command-line arguments
  */
  char *filename = nullptr;
  bool do_help = false;
  bool arg_error = false;
  string cache_config;
  for (int i = 1; i < argc; i++) {
    string arg(argv[i]);
    if (arg.rfind("-", 0) == 0) {
      if (arg == "-h" || arg == "--help")
        do_help = true;
      else if (arg == "--cache") {
        i++;
        if (i >= argc)
          arg_error = true;
        else
          cache_config = argv[i];
      } else
        arg_error = true;
    } else {
      if (filename == nullptr)
        filename = argv[i];
      else
        arg_error = true;
    }
  }
  /* Display error message if appropriate */
  if (arg_error || do_help || filename == nullptr) {
    cerr << "usage " << argv[0] << " [-h] [--cache CACHE] filename" << endl
         << endl;
    cerr << "Simulate E20 cache" << endl << endl;
    cerr << "positional arguments:" << endl;
    cerr << "  filename    The file containing machine code, typically with "
            ".bin suffix"
         << endl
         << endl;
    cerr << "optional arguments:" << endl;
    cerr << "  -h, --help  show this help message and exit" << endl;
    cerr << "  --cache CACHE  Cache configuration: "
            "size,associativity,blocksize (for one"
         << endl;
    cerr << "                 cache) or" << endl;
    cerr << "                 "
            "size,associativity,blocksize,size,associativity,blocksize"
         << endl;
    cerr << "                 (for two caches)" << endl;
    return 1;
  }

  // Open machine-code file
  ifstream f(filename);
  if (!f.is_open()) {
    cerr << "Can't open file " << filename << endl;
    return 1;
  }

  // Initialize processor state
  uint16_t pc = 0;
  uint16_t regArr[NUM_REGS] = {0};
  uint16_t mem[MEM_SIZE] = {0};

  // Load program into memory
  load_machine_code(f, mem);

  /* parse cache config */
  if (cache_config.size() > 0) {
    vector<int> parts;
    size_t pos;
    size_t lastpos = 0;
    while ((pos = cache_config.find(",", lastpos)) != string::npos) {
      parts.push_back(stoi(cache_config.substr(lastpos, pos)));
      lastpos = pos + 1;
    }
    parts.push_back(stoi(cache_config.substr(lastpos)));
    if (parts.size() == 3) {
      int L1size = parts[0];
      int L1assoc = parts[1];
      int L1blocksize = parts[2];

      // TODO: execute E20 program and simulate one cache here
      Cache L1;
      init_cache(L1, "L1", L1size, L1assoc, L1blocksize);
      Cache *L2ptr = nullptr;
      // Execute E20 program and simulate one cache
      sim(pc, regArr, mem, L1, L2ptr);

    } else if (parts.size() == 6) {
      int L1size = parts[0];
      int L1assoc = parts[1];
      int L1blocksize = parts[2];
      int L2size = parts[3];
      int L2assoc = parts[4];
      int L2blocksize = parts[5];

      // TODO: execute E20 program and simulate two caches here
      Cache L1;
      Cache L2;
      init_cache(L1, "L1", L1size, L1assoc, L1blocksize);
      init_cache(L2, "L2", L2size, L2assoc, L2blocksize);
      Cache *L2ptr = &L2;
      // Execute E20 program and simulate two caches
      sim(pc, regArr, mem, L1, L2ptr);

    } else {
      cerr << "Invalid cache config" << endl;
      return 1;
    }
  }

  return 0;
}
// ra0Eequ6ucie6Jei0koh6phishohm