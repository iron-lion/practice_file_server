add_custom_target(client)

include_directories(JOB ${tipdir}/common/job)
set(EXTRA_LIBS JOB)

add_executable(CLA client.c)
target_link_libraries(CLA ${EXTRA_LIBS})
add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/bin/CLA
                    COMMAND rm -f ${PROJECT_SOURCE_DIR}/bin/CLA
                    COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/CLA ${PROJECT_SOURCE_DIR}/bin
                    )
add_custom_target(symlink.CLA ALL DEPENDS ${PROJECT_SOURCE_DIR}/bin/CLA)

add_dependencies(client symlink.CLA)
add_dependencies(client CLA)
