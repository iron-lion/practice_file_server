file(GLOB Sources "*.c")

add_library(JOB job.c)

add_library(CPYTHON python_caller.c)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(CPYTHON ${PYTHON_LIBRARIES})
