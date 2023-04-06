# Create an INTERFACE library for our C module.
add_library(tt_driver INTERFACE)

# Add our source files to the lib
target_sources(tt_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/tt_driver_module.c
    ${CMAKE_CURRENT_LIST_DIR}/tt.c
)

# Add the current directory as an include directory.
target_include_directories(tt_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE tt_driver)
