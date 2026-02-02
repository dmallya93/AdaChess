/**
 * AdaChess - Smart Chess Engine
 *
 * Engine metadata and identification functions.
 * C++ port of the original Ada implementation (chess-info.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_INFO_HPP
#define ADACHESS_INFO_HPP

#include <string_view>

namespace chess::info {

/**
 * Get the name of the engine.
 * @return The pure name of the engine ("AdaChess").
 */
[[nodiscard]] constexpr std::string_view engine_name() noexcept {
    return "AdaChess";
}

/**
 * Get the current motto of the engine.
 * @return The motto of this engine.
 */
[[nodiscard]] constexpr std::string_view engine_motto() noexcept {
    return "Smart Chess Engine";
}

/**
 * Get the version number of the engine.
 * The output is composed of numbers representing the major version,
 * minor version, and status.
 * @return The version number of the engine.
 */
[[nodiscard]] constexpr std::string_view version() noexcept {
    return "4.0";
}

/**
 * Get the author of AdaChess.
 * @return The author's name.
 */
[[nodiscard]] constexpr std::string_view author() noexcept {
    return "Alessandro Iavicoli";
}

/**
 * Get the official email address of the engine.
 * @return The contact email address.
 */
[[nodiscard]] constexpr std::string_view e_mail() noexcept {
    return "adachess@gmail.com";
}

/**
 * Get the official internet site of the engine.
 * @return The GitHub URL.
 */
[[nodiscard]] constexpr std::string_view site() noexcept {
    return "https://github.com/adachess/AdaChess";
}

/**
 * Get the AdaChess logo as ASCII art for console display.
 * @return The ASCII art logo string.
 */
[[nodiscard]] constexpr std::string_view console_logo() noexcept {
    return 
        "                                      .(@).                                     \n"
        "                          (@@@@@@@@@@@@&&&@@@@@@@@@@@@,                         \n"
        "                     @@@@@@                           @@@@@@                    \n"
        "                 @@@@@                                     @@@@@                \n"
        "              @@@@                                             @@@@             \n"
        "           @@@@                       @@@@@                       @@@#          \n"
        "         @@@@                  (      @@@@@      )                  @@@@        \n"
        "       @@@@                    @@@@@@@@@@@@@@@@@@@                    @@@%      \n"
        "      @@@                       .@@@@@@@@@@@@@@@                        @@@     \n"
        "    (@@                           @@@@@@@@@@@@@                          ,@@    \n"
        "   @@@                            @@@@@@@@@@@@                             @@%  \n"
        "  @@@                              ,,,,,,,,,,,                              @@  \n"
        "  @@                            @@@@@@@@@@@@@@@@@                           ,@@ \n"
        " @@@                             #@@@@@@@@@@@@@*                             @@@\n"
        " @@                                ,,,,,,,,,,,                               *@@\n"
        " @@                                @@@@@@@@@@@                                @@\n"
        "@@@                                @@@@@@@@@@@                                @@\n"
        " @@                                @@@@@@@@@@@                                @@\n"
        " @@                                @@@@@@@@@@@                               /@@\n"
        " @@@                              @@@@@@@@@@@@%                              @@@\n"
        "  @@                              @@@@@@@@@@@@@                             /@@ \n"
        "  #@@                            *@@@@@@@@@@@@@                             @@  \n"
        "   @@@                           @@@@@@@@@@@@@@@                           @@(  \n"
        "    .@@                        @@@@@@@@@@@@@@@@@@@                       (@@    \n"
        "      @@@                   @@@@@@@@@@@@@@@@@@@@@@@@@                   @@@     \n"
        "       @@@@               @@@@@@@@@@@@@@@@@@@@@@@@@@@@@               @@@/      \n"
        "         @@@@             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@             @@@@        \n"
        "           #@@@           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@           @@@,          \n"
        "              @@@@                                             @@@@             \n"
        "                 @@@@@                                     @@@@@                \n"
        "                     @@@@@@,                         *@@@@@@                    \n"
        "                           @@@@@@@@@@@@@@@@@@@@@@@@@@@                          \n";
}

/**
 * Get a formatted version string including engine name, version, and motto.
 * @return A formatted identification string.
 */
[[nodiscard]] inline std::string full_version_string() {
    std::string result;
    result.reserve(64);
    result += engine_name();
    result += " ";
    result += version();
    result += " - ";
    result += engine_motto();
    return result;
}

} // namespace chess::info

#endif // ADACHESS_INFO_HPP
