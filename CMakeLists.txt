cmake_minimum_required(VERSION 3.20)
project(AdaChess VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prefer Clang for development
if(NOT CMAKE_CXX_COMPILER_ID)
    find_program(CLANG_CXX clang++)
    if(CLANG_CXX)
        set(CMAKE_CXX_COMPILER ${CLANG_CXX})
    endif()
endif()

# Enable modern C++ warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4 /permissive-)
endif()

# Warnings as errors (optional, can be disabled for development)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
if(WARNINGS_AS_ERRORS)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

# Set up clang-tidy (automatic during build)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy-20" "clang-tidy")
if(CLANG_TIDY_EXE AND ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
else()
    message(STATUS "clang-tidy not found or disabled")
endif()

# Sanitizer support: Enabled by default in Debug builds
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" ON)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Find dependencies
find_package(GTest)
if(NOT GTest_FOUND)
    # Try pkg-config as fallback
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GTEST gtest_main gtest)
    endif()
endif()

# If GTest still not found, try manual find
if(NOT GTest_FOUND AND NOT GTEST_FOUND)
    find_path(GTEST_INCLUDE_DIR gtest/gtest.h PATHS /usr/include)
    find_library(GTEST_LIBRARY NAMES gtest PATHS /usr/lib /usr/lib64 /usr/lib/x86_64-linux-gnu)
    find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS /usr/lib /usr/lib64 /usr/lib/x86_64-linux-gnu)
    find_library(PTHREAD_LIBRARY pthread)

    if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY AND GTEST_MAIN_LIBRARY)
        set(GTEST_FOUND TRUE)
        set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
        set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${PTHREAD_LIBRARY})
        set(GTEST_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARIES})
        message(STATUS "Found GTest manually: ${GTEST_LIBRARIES}")
    endif()
endif()

# Core library (now includes chessboard, attack detection, move system, and zobrist hashing)
add_library(adachess_core STATIC
    src/board/chessboard.cpp
    src/board/attacks.cpp
    src/board/move.cpp
    src/core/zobrist.cpp
)
target_include_directories(adachess_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Tests
if(GTest_FOUND OR GTEST_FOUND)
    enable_testing()

    add_executable(adachess_tests
        tests/test_main.cpp
        tests/test_color.cpp
        tests/test_piece.cpp
        tests/test_score.cpp
        tests/test_depth.cpp
        tests/test_node.cpp
        tests/test_hash.cpp
        tests/test_pin.cpp
        tests/test_square.cpp
        tests/test_direction.cpp
        tests/test_chessboard.cpp
        tests/test_attacks.cpp
        tests/test_move.cpp
    )

    target_link_libraries(adachess_tests PRIVATE adachess_core)

    if(GTest_FOUND)
        target_link_libraries(adachess_tests PRIVATE GTest::gtest GTest::gtest_main)
    else()
        target_include_directories(adachess_tests PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(adachess_tests PRIVATE ${GTEST_MAIN_LIBRARIES})
    endif()

    add_test(NAME unit_tests COMMAND adachess_tests)
else()
    message(WARNING "GTest not found - tests will not be built")
endif()

# Custom targets for code formatting
find_program(CLANG_FORMAT_EXE NAMES "clang-format-20" "clang-format")
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/include/*.hpp
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/tests/*.cpp
    )
    if(ALL_SOURCE_FILES)
        add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
            COMMENT "Formatting source files with clang-format"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
        add_custom_target(format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCE_FILES}
            COMMENT "Checking code formatting"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()
endif()

# Custom target for manual clang-tidy run
if(CLANG_TIDY_EXE)
    file(GLOB_RECURSE TIDY_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/tests/*.cpp
    )
    if(TIDY_FILES)
        add_custom_target(run-clang-tidy
            COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${TIDY_FILES}
            COMMENT "Running clang-tidy on source files"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()
endif()
