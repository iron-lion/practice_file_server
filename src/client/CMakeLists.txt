add_custom_target(client)

add_executable(CLA client.c)
add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/bin/CLA
                    COMMAND rm -f ${PROJECT_SOURCE_DIR}/bin/CLA
                    COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/CLA ${PROJECT_SOURCE_DIR}/bin
                    )
add_custom_target(symlink.CLA ALL DEPENDS ${PROJECT_SOURCE_DIR}/bin/CLA)

add_dependencies(client symlink.CLA)
add_dependencies(client CLA)
