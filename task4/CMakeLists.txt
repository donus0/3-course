add_executable(task4 main.cpp)

target_include_directories(task4 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task4 PRIVATE -Wall -Wextra -Wpedantic)

