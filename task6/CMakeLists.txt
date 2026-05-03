add_executable(task6 main.cpp)

target_include_directories(task6 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task6 PRIVATE -Wall -Wextra -Wpedantic)

