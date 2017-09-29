add_custom_target(server)

set(CMAKE_USE_PTHREADS_INIT TRUE)

add_executable(SERV server.c)
target_link_libraries(SERV ${CMAKE_THREAD_LIBS_INIT})
add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/bin/SERV
                    COMMAND rm -f ${PROJECT_SOURCE_DIR}/bin/SERV
                    COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/SERV ${PROJECT_SOURCE_DIR}/bin
                    )
add_custom_target(symlink.SERV ALL DEPENDS ${PROJECT_SOURCE_DIR}/bin/SERV)

add_dependencies(server symlink.SERV)
add_dependencies(server SERV)
