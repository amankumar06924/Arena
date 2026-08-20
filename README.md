# Custom Arena Allocator in C

A custom arena memory allocator written from scratch in C as a low-level
memory-management learning project.

## What Is an Arena Allocator?

An arena allocator is a memory allocation strategy where a large region of
memory is obtained once and smaller allocations are made from that region.

Instead of individually allocating and freeing every object, the arena
maintains a current position and moves that position forward as allocations
are requested.

For more background on arena allocators:
https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator

## Current Implementation

This version of the arena allocator uses `malloc()` as its underlying memory
source.

The arena first allocates memory for the arena structure itself and then
allocates a contiguous memory buffer using `malloc()`:

/**************************************************************************************************************/

# Custom Virtual Memory Arena Allocator in C

A custom arena memory allocator written from scratch in C as a low-level
memory-management learning project.

This is the second version of my arena allocator. Unlike the first version,
this implementation uses virtual memory reservation and commitment instead of
using `malloc()` for the main arena memory buffer.

## Version 2 — Virtual Memory Arena using mmap()

This version goes one level lower and uses the operating system's virtual
memory facilities.

The arena first reserves a large virtual address range and initially commits
only a small portion of that range.

```text
arena_create()

     │
     ├── malloc(sizeof(arena))
     │
     └── reserve virtual address space
              │
              ▼
        ┌──────────────────────────────┐
        │      Reserved Address Space  │
        │                              │
        │  ┌──────────────┐            │
        │  │   Committed  │            │
        │  │    Memory    │            │
        │  └──────────────┘            │
        │                              │
        │       Reserved               │
        │       but not                │
        │       committed              │
        │                              │
        └──────────────────────────────┘