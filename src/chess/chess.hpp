// ---------------------------------------------------------------------------
// AdaChess – Root namespace and common definitions
// Translated from: chess.ads
// ---------------------------------------------------------------------------
// AdaChess - Smart Chess Engine
//
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// Email: adachess@gmail.com - Web Page: https://github.com/adachess/AdaChess
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// ---------------------------------------------------------------------------
#pragma once

#include <stdexcept>

namespace chess {

/// Exception type corresponding to Ada's Not_Implemented.
class NotImplemented : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace chess
