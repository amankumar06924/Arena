# Custom Arena Allocator in C

A custom arena memory allocator written from scratch in C as a low-level
memory-management learning project.

## What Is an Arena Allocator?

An arena allocator is a memory allocation strategy where a large block of memory
is obtained once and smaller allocations are made from that block.

For more background on arena allocators:

https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator

## Current Implementation

This version of the arena allocator uses `malloc()` as its underlying memory
source.

The arena first allocates memory for the arena structure itself and then
allocates a contiguous memory buffer using `malloc()`:

```text
arena_create()
     │
     ├── malloc(sizeof(arena))
     │
     └── malloc(capacity)
              │
              ▼
        Arena memory buffer