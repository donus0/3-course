add_executable(task5 main.cpp)

target_include_directories(task5 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task5 PRIVATE -Wall -Wextra -Wpedantic)

