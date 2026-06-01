add_executable(task9 main.cpp)

target_include_directories(task9 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task9 PRIVATE -Wall -Wextra -Wpedantic)
