// ---------------------------------------------------------------------------
// AdaChess – Precondition / Postcondition / Invariant Macros
// ---------------------------------------------------------------------------
// Translates Ada's Pre/Post aspects and implicit range checks into C++20.
//
// In Debug / Profile builds (NDEBUG not defined): expands to assert().
// In Release / Speed builds (NDEBUG defined):     expands to nothing.
//
// Design Decision #4: We choose the safe default (no [[assume]]) for
// Release builds.  Speed builds may opt into [[assume]] separately.
// ---------------------------------------------------------------------------
#pragma once

#include <cassert>

#ifdef NDEBUG
    #define ADACHESS_PRECOND(expr)    ((void)0)
    #define ADACHESS_POSTCOND(expr)   ((void)0)
    #define ADACHESS_INVARIANT(expr)  ((void)0)
#else
    #define ADACHESS_PRECOND(expr)    assert(expr)
    #define ADACHESS_POSTCOND(expr)   assert(expr)
    #define ADACHESS_INVARIANT(expr)  assert(expr)
#endif
