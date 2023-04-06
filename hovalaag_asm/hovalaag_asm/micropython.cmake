# Create an INTERFACE library for our C module.
add_library(hovalaag_asm INTERFACE)

# Add our source files to the lib
target_sources(hovalaag_asm INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hovalaag_module.c
    ${CMAKE_CURRENT_LIST_DIR}/hovalaag_assembler.c
)

# Add the current directory as an include directory.
target_include_directories(hovalaag_asm INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE hovalaag_asm)
