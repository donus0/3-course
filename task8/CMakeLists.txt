add_executable(task8 main.cpp)

target_include_directories(task8 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task8 PRIVATE -Wall -Wextra -Wpedantic)

