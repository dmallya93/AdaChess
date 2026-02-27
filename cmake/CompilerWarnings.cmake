# ---------------------------------------------------------------------------
# AdaChess – Compiler Warning Flags
# ---------------------------------------------------------------------------
# Provides the function adachess_set_warnings(<target>) which applies
# strict warning flags appropriate to the detected compiler.
#
# Compiler version requirements (from the Modernization Spec):
#   GCC   >= 11
#   Clang >= 12
#   MSVC  (Visual Studio 2022, i.e. _MSC_VER >= 1930)
# ---------------------------------------------------------------------------

include_guard(GLOBAL)

# ---------------------------------------------------------------------------
# Compiler version checks
# ---------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11")
        message(FATAL_ERROR
            "AdaChess requires GCC >= 11 for C++20 support. "
            "Detected GCC ${CMAKE_CXX_COMPILER_VERSION}.")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12")
        message(FATAL_ERROR
            "AdaChess requires Clang >= 12 for C++20 support. "
            "Detected Clang ${CMAKE_CXX_COMPILER_VERSION}.")
    endif()
elseif(MSVC)
    if(MSVC_VERSION LESS 1930)
        message(FATAL_ERROR
            "AdaChess requires Visual Studio 2022 (MSVC >= 19.30). "
            "Detected MSVC ${MSVC_VERSION}.")
    endif()
endif()

# ---------------------------------------------------------------------------
# adachess_set_warnings(<target>)
# ---------------------------------------------------------------------------
# Apply strict warning flags to the given target.  Called from the top-level
# CMakeLists.txt when ADACHESS_STRICT_WARNINGS is ON.
# ---------------------------------------------------------------------------
function(adachess_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /WX
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
           OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
        )
    endif()
endfunction()
