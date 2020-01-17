cmake_minimum_required(VERSION 3.0.2)
project(k10)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cc")

# Generate the Standard.h file
if (${CMAKE_SOURCE_DIR}/static/boards/Standard.board IS_NEWER_THAN ${CMAKE_SOURCE_DIR}/include/Board/generated/Standard.h)
    execute_process(
        COMMAND ./scripts/generate-boards --names Standard
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

add_executable(main ${SOURCES})
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)
