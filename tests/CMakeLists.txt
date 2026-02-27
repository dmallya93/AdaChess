# ---------------------------------------------------------------------------
# AdaChess test targets
# ---------------------------------------------------------------------------

include(GoogleTest)

# Unit tests
add_executable(adachess_unit_tests
    unit/types_tests.cpp
    unit/board_tests.cpp
    unit/string_lib_tests.cpp
    unit/protocol_tests.cpp
)

target_link_libraries(adachess_unit_tests PRIVATE
    adachess_core
    GTest::gtest_main
)

gtest_discover_tests(adachess_unit_tests)
