# Memory Allocators

A collection of custom memory allocator implementations in C++, built to learn low-level memory management, alignment, and performance characteristics.

## What's Here

### `bump/`
**The main implementation** — a non-owning bump allocator written in C++.

- `bump.h` — allocator interface
- `bump.cpp` — implementation
- `ideation/` — exploratory code and experiments while figuring out how bump allocation works (not production code)

The bump allocator:
- manages a user-provided memory buffer (non-owning)
- allocates memory linearly in O(1) time
- supports custom alignment requirements
- provides instant reset via pointer manipulation
- does not own or allocate the underlying buffer

### `initial-tinkering/`
Early experiments before learning allocators properly . Just me learning about allocator concepts before understanding what I was actually doing. Kept for historical context.

## Philosophy

- **Explicit over implicit** — no hidden behavior
- **Mechanical invariants** — enforce correctness through design
- **Separation of concerns** — memory allocation ≠ object construction
- **Minimal and honest** — small implementations that do exactly what they claim

## Status

Bump allocator complete  
Future allocators (arena, pool, freelist) planned  
Tests and benchmarks coming eventually  

## Purpose

This is a **learning repository**, not a production library. It exists to:
- understand how allocators actually work
- explore performance trade-offs
- build intuition for low-level memory management

Don't use this in production. Use your platform's allocator or a battle-tested library.