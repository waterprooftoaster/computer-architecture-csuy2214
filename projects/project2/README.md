Andy Wu
cw4483@nyu.edu

## State of the Work

All required functionality for Project Two is fully implemented:
  1. The simulator correctly executes the E20 program
  2. Models L1 and optional L2 caches
  3. Applies LRU replacement
  4. Supports write-through + write-allocate semantics
  5. Logs all cache activity in the format specified by the examples

Instruction fetches are correctly executed without generating log output. Only load (lw) and store (sw) instructions generate entries in the cache log as described in the project outline.

There are no known bugs, crashes, or missing behaviors.

## Design Rationale

1. Cache rows are implemented as vector<int> storing tags. This closely follows the starter file structure and enables simple LRU maintenance by shifting elements.
2. The LRU policy is implemented by moving the accessed tag to position 0 (MRU) and shifting older entries backward.
3. The CPU is copied directly from project 1 and the cache simulation is kept logically separate. The CPU uses the cache only through cache_load_with_fill and cache_store_access, which keeps the simulator readable and ensures caching does not pollute CPU logic.
4. The implementation strictly follows the project outline:
  - Loads: check L1 → possibly check L2 → fill L2 (if miss) → fill L1.

  - Stores: always write memory, and apply write-allocate, updating both caches.

  - This consistency minimizes bugs and matches the example sequences.

## Potential Weaknesses

1. The vector-shift LRU implementation is O(assoc) per access.
2. Error handling is minimal.
