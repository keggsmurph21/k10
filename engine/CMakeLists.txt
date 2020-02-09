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
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=bugprone-*,google-*,readability-*,misc-*,modernize-*,performance-*,portability-*,-modernize-use-trailing-return-type,-google-readability-namespace-comments)

include_directories(include)

add_library(k10 SHARED
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

add_executable(test_
    src/test.cc
    src/Board/Test/Graph.cc
    src/Board/Test/Node.cc
    src/Board/Test/Port.cc
    src/Scenario/Test/Scenario.cc
)
target_compile_options(test_ PRIVATE -Werror -g)
