cmake_minimum_required(VERSION 3.0.2)
project(k10)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cc")

add_executable(main ${SOURCES})
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)
