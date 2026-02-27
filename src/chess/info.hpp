// ---------------------------------------------------------------------------
// AdaChess – Engine information
// Translated from: chess-info.ads
// ---------------------------------------------------------------------------
#pragma once

#include <string_view>

namespace chess {

/// Expose the name of the engine.
constexpr std::string_view Engine_Name() noexcept {
    return "AdaChess";
}

/// Expose the current motto of the engine.
constexpr std::string_view Engine_Motto() noexcept {
    return "Smart Chess Engine";
}

/// Export the current version of the engine.
constexpr std::string_view Version() noexcept {
    return "4.0";
}

/// Export the author of AdaChess.
constexpr std::string_view Author() noexcept {
    return "Alessandro Iavicoli";
}

/// Export the official email address of the engine.
constexpr std::string_view E_Mail() noexcept {
    return "adachess@gmail.com";
}

/// Expose the official internet site of the engine.
constexpr std::string_view Site() noexcept {
    return "https://github.com/adachess/AdaChess";
}

/// Draw the AdaChess logo as ASCII art.
constexpr std::string_view Console_Logo() noexcept {
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

} // namespace chess
