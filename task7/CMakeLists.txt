add_executable(task7 main.cpp)

target_include_directories(task7 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(task7 PRIVATE -Wall -Wextra -Wpedantic)

