add_executable(task10 main.cpp)

target_include_directories(task10 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task10 PRIVATE -Wall -Wextra -Wpedantic)
