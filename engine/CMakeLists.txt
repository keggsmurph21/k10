cmake_minimum_required(VERSION 3.0.2)
project(k10)

# Generate the Standard.h file
if (${CMAKE_SOURCE_DIR}/static/boards/Standard.board IS_NEWER_THAN ${CMAKE_SOURCE_DIR}/include/Board/generated/Standard.h)
    execute_process(
        COMMAND ./scripts/generate-boards --names Standard
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(k10 SHARED
    src/Board/Base.cc
    src/Board/Graph.cc
    src/Board/Node.cc
    src/Board/Orientation.cc
    src/Core/Random.cc
    src/Game/Dice.cc
    src/Game/Game.cc
    src/Game/Player.cc
    src/Scenario/Scenario.cc
)
target_compile_options(k10 PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

link_directories(build)
link_libraries(k10)

add_executable(main src/main.cc)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

FILE(GLOB k10_TEST_SOURCES src/*/Test/*.cc)
add_executable(test_ ${k10_TEST_SOURCES} src/test.cc)
target_compile_options(test_ PRIVATE -Werror)
