# AdaChess main executable

add_executable(adachess
    main.cpp
)

target_link_libraries(adachess PRIVATE
    adachess_core
)

# Set output name
set_target_properties(adachess PROPERTIES
    OUTPUT_NAME "adachess"
)
