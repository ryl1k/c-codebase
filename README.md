C Codebase Library
Overview

This project provides a comprehensive library of C macros, utility functions, data structures, and memory management tools to streamline and enhance low-level programming. Designed for versatility and efficiency, the library includes advanced support for:

    Mathematical operations
    UTF string manipulation
    Memory management
    Common data structures like linked lists, stacks, and queues

Features
Macros for Simplified Development

    Memory management: MemoryZero, MemoryCopy, etc.
    Debugging utilities: Assert, AssertBreak.
    Mathematical operations: Clamp, Min, Max.

Data Structures

    Doubly linked lists (DLL).
    Singly linked lists (SLL).
    Stacks and queues with macro-based operations.

Memory Management

    Custom memory arenas (Arena, Arena64) for efficient allocation.

String Handling

    UTF-8 and UTF-16 support.
    Conversion between UTF formats.
    Utilities for creating and managing string literals.

Mathematics

    Common mathematical functions for floats and doubles.
    Vector and interval operations (V2F32, I2F32).

Utility Functions

    Platform and architecture detection.
    Symbolic constants for system-level programming.

Code Structure

    base.h: Header file containing macros, type definitions, and function declarations.
    base.c: Implementation of utility functions, including memory, math, and string operations.
    main.c: Example and test cases for edge scenarios of the library functions.

Build and Run
Prerequisites

    A C compiler (e.g., GCC, Clang, MSVC).
    Optional: Makefile for automating the build process.

Steps

    Clone the repository:

git clone https://github.com/your-username/c-codebase-library.git
cd c-codebase-library

Compile and build:

gcc -o program main.c base.c

Run the program:

    ./program

Examples
Memory Arena Allocation

Arena arena = arena_create(1024);
void* block = arena_alloc(&arena, 256);
arena_reset(&arena);
arena_destroy(&arena);

UTF-8 String Manipulation

String utf8_str = utf8_create_literal("Hello, 世界!");
printf("String: %s, Length: %d\n", utf8_str.data, utf8_str.length);

Math Utilities

F32 clamped = Clamp(0.0f, 5.0f, 10.0f); // Output: 5.0

License

This project is open-source and available under the MIT License. See LICENSE for more details.
Contributions

Contributions are welcome! Feel free to submit issues or pull requests for improvements or bug fixes.
