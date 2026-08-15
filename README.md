# Custom Arena Allocator in C

A custom arena memory allocator written from scratch in C as a low-level
memory-management learning project.

<!-- The goal of this project is not to create a production-ready allocator.
Instead, it is being built step by step to understand how memory allocation,
alignment, pointer arithmetic, integer overflow, temporary allocation scopes,
and eventually virtual memory work at a low level.

The allocator is being developed incrementally, with each feature implemented
and tested individually.

---

## Project Status

### Currently Implemented

- [x] Arena creation
- [x] Arena destruction
- [x] Basic bump-pointer allocation
- [x] Capacity/bounds checking
- [x] Memory alignment
- [x] Power-of-two alignment validation
- [x] Alignment-aware allocation
- [x] `arena_pop()`
- [x] `arena_pop_to()`
- [x] `arena_clear()`
- [x] Arena position tracking
- [x] Typed structure allocation
- [x] Array allocation
- [x] Integer-overflow protection for array allocation
- [x] Temporary allocation scopes
- [x] Zero-initialized allocation
- [x] Basic testing

### Planned

- [ ] Virtual memory backed arena
- [ ] Memory reservation
- [ ] Incremental memory commitment
- [ ] Linux implementation using `mmap`
- [ ] Windows implementation using `VirtualAlloc`
- [ ] Memory decommit
- [ ] Platform abstraction layer
- [ ] Performance measurements
- [ ] Use the allocator in a larger C project

--- -->

# What Is an Arena Allocator?

An arena allocator is a memory allocation strategy where a large block of memory
is obtained once and smaller allocations are made from that block.

https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator