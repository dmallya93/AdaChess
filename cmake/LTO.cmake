# ---------------------------------------------------------------------------
# AdaChess – Link-Time Optimization (LTO) Support
# ---------------------------------------------------------------------------
# Provides the function adachess_enable_lto() which checks for IPO/LTO
# support and enables it project-wide when available.
#
# Called from the top-level CMakeLists.txt when ADACHESS_ENABLE_LTO is ON.
# ---------------------------------------------------------------------------

include_guard(GLOBAL)

function(adachess_enable_lto)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _lto_supported OUTPUT _lto_output)
    if(_lto_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE PARENT_SCOPE)
        message(STATUS "AdaChess: LTO enabled")
    else()
        message(WARNING "AdaChess: LTO not supported by this toolchain: ${_lto_output}")
    endif()
endfunction()
