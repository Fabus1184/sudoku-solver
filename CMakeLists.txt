cmake_minimum_required(VERSION 3.0.0)
project(Sudoku VERSION 0.1.0)

set(CMAKE_C_STANDARD 17)

add_executable(sudoku main.c)

target_compile_options(sudoku PRIVATE -Ofast -Wall -Wextra -Werror -Wno-format)
