/**
 * AdaChess - Smart Chess Engine
 *
 * Evaluation types implementation.
 * C++ port of the original Ada implementation (chess-engine-evaluations.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/engine/evaluations/evaluations.hpp"

namespace chess::engine::evaluations {

// Global resign mode flag
bool g_resign_mode = false;

} // namespace chess::engine::evaluations
