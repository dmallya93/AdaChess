# ---------------------------------------------------------------------------
# AdaChess – Build Type Configuration
# ---------------------------------------------------------------------------
# Defines the four build configurations specified in the Modernization Spec:
#
#   Debug   – -O0/-Og, full debug info, assertions active
#   Release – -O3, LTO optional, NDEBUG defined
#   Speed   – As Release, plus aggressive inlining and loop unrolling
#   Profile – -O2/-O3 with frame pointers and symbols for profilers
#
# Also registers Speed and Profile as valid build types for both
# single-config and multi-config generators.
# ---------------------------------------------------------------------------

include_guard(GLOBAL)

# ---------------------------------------------------------------------------
# Speed build type flags
# ---------------------------------------------------------------------------
if(MSVC)
    set(CMAKE_C_FLAGS_SPEED     "/O2 /DNDEBUG"          CACHE STRING "C flags for Speed build")
    set(CMAKE_CXX_FLAGS_SPEED   "/O2 /DNDEBUG"          CACHE STRING "CXX flags for Speed build")
    set(CMAKE_EXE_LINKER_FLAGS_SPEED ""                  CACHE STRING "Linker flags for Speed build")
    set(CMAKE_SHARED_LINKER_FLAGS_SPEED ""               CACHE STRING "Shared linker flags for Speed build")
else()
    set(CMAKE_C_FLAGS_SPEED     "-O3 -DNDEBUG -finline-functions -funroll-loops"
        CACHE STRING "C flags for Speed build")
    set(CMAKE_CXX_FLAGS_SPEED   "-O3 -DNDEBUG -finline-functions -funroll-loops"
        CACHE STRING "CXX flags for Speed build")
    set(CMAKE_EXE_LINKER_FLAGS_SPEED ""                  CACHE STRING "Linker flags for Speed build")
    set(CMAKE_SHARED_LINKER_FLAGS_SPEED ""               CACHE STRING "Shared linker flags for Speed build")
endif()

# ---------------------------------------------------------------------------
# Profile build type flags
# ---------------------------------------------------------------------------
if(MSVC)
    set(CMAKE_C_FLAGS_PROFILE   "/O2 /Zi /DNDEBUG"      CACHE STRING "C flags for Profile build")
    set(CMAKE_CXX_FLAGS_PROFILE "/O2 /Zi /DNDEBUG"      CACHE STRING "CXX flags for Profile build")
    set(CMAKE_EXE_LINKER_FLAGS_PROFILE "/DEBUG"          CACHE STRING "Linker flags for Profile build")
    set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "/DEBUG"       CACHE STRING "Shared linker flags for Profile build")
else()
    set(CMAKE_C_FLAGS_PROFILE   "-O2 -g -DNDEBUG -fno-omit-frame-pointer"
        CACHE STRING "C flags for Profile build")
    set(CMAKE_CXX_FLAGS_PROFILE "-O2 -g -DNDEBUG -fno-omit-frame-pointer"
        CACHE STRING "CXX flags for Profile build")
    set(CMAKE_EXE_LINKER_FLAGS_PROFILE ""                CACHE STRING "Linker flags for Profile build")
    set(CMAKE_SHARED_LINKER_FLAGS_PROFILE ""             CACHE STRING "Shared linker flags for Profile build")
endif()

# ---------------------------------------------------------------------------
# Register custom build types
# ---------------------------------------------------------------------------
get_property(_is_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi)
    # Multi-config generators (Visual Studio, Xcode, Ninja Multi-Config)
    if(NOT "Speed" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Speed)
    endif()
    if(NOT "Profile" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
    endif()
else()
    # Single-config generators (Ninja, Unix Makefiles)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release Speed Profile RelWithDebInfo MinSizeRel)
endif()
