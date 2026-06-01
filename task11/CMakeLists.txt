add_executable(task11 main.cpp)

target_include_directories(task11 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task11 PRIVATE -Wall -Wextra -Wpedantic)
