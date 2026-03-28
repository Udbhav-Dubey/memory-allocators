# Memory Allocators
A collection of custom memory allocator implementations in C++, built to learn
low-level memory management, alignment, and performance characteristics.

## What's Here

### `bump/`
A non-owning bump allocator written in C++.
- `bump.h` — allocator interface
- `bump.cpp` — implementation
- `ideation/` — exploratory code and experiments (not production code)

The bump allocator:
- manages a user-provided memory buffer (non-owning)
- allocates memory linearly in O(1) time
- supports custom alignment requirements
- provides instant reset via pointer manipulation
- does not own or allocate the underlying buffer

### `areana/`
A growable arena allocator written in C++.
- `areana.h` — allocator interface
- `areana.cpp` — implementation
- `ideation/` — exploratory code and experiments (not production code)

The arena allocator:
- owns and manages its own memory via internal chunks
- chains new chunks automatically when current chunk is exhausted
- allocates linearly in O(1) amortized time
- supports custom alignment requirements
- bulk-frees on destruction

### `initial-tinkering/`
Early experiments before learning allocators properly. Kept for historical context.

## Benchmarks
100,000 allocations of a 12-byte struct (`int` + `double`), compiled with `-O2`.

| Allocator     | Time        |
|---------------|-------------|
| Bump          | ~231 µs     |
| Arena         | ~241 µs     |
| `new` (heap)  | ~2900 µs    |

Arena is ~12x faster than `new` under `-O2`.

## Philosophy
- **Explicit over implicit** — no hidden behavior
- **Mechanical invariants** — enforce correctness through design
- **Separation of concerns** — memory allocation ≠ object construction
- **Minimal and honest** — small implementations that do exactly what they claim

## Status
- Bump allocator — complete
- Arena allocator — complete
- Pool allocator — planned
- Slab allocator — planned
- Free-list allocator — planned

## Purpose
This is a **learning repository**, not a production library. It exists to:
- understand how allocators actually work
- explore performance trade-offs
- build intuition for low-level memory management

Don't use this in production. Use your platform's allocator or a battle-tested library.
