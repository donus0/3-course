add_executable(task2 main.cpp)

target_include_directories(task2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(task2 PRIVATE -Wall -Wextra -Wpedantic)

