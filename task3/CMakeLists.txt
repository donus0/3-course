add_executable(task3 main.cpp)

target_include_directories(task3 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task3 PRIVATE -Wall -Wextra -Wpedantic)

