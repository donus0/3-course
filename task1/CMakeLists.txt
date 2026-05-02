add_executable(task1 main.cpp)

target_include_directories(task1 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task1 PRIVATE -Wall -Wextra -Wpedantic)

