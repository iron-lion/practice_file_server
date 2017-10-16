add_custom_target(server)

include_directories(${PROJECT_SOURCE_DIR}/src/common)
set(EXTRA_LIBS JOB)

add_library(WTHR server_wthr.c)
target_link_libraries(WTHR ${EXTRA_LIBS})
set(EXTRA_LIBS WTHR)

add_executable(SERV server_cthr.c)
target_link_libraries(SERV ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(SERV ${EXTRA_LIBS})

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/bin/SERV
                    COMMAND rm -f ${PROJECT_SOURCE_DIR}/bin/SERV
                    COMMAND ln -s ${CMAKE_CURRENT_BINARY_DIR}/SERV ${PROJECT_SOURCE_DIR}/bin
                    )
add_custom_target(symlink.SERV ALL DEPENDS ${PROJECT_SOURCE_DIR}/bin/SERV)

add_dependencies(server WTHR)
add_dependencies(server symlink.SERV)
add_dependencies(server SERV)
