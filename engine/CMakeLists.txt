cmake_minimum_required(VERSION 3.0.2)
project(k10)

set(CMAKE_CXX_STANDARD 17)

find_program(CLANG_TIDY NAMES clang-tidy)
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})

include_directories(src)

add_library(k10 SHARED
    src/Board/Direction.cc
    src/Board/Graph.cc
    src/Board/Node.cc
    src/Board/NodeType.cc
    src/Board/Orientation.cc
    src/Board/Parser.cc
    src/Board/Port.cc
    src/Core/Action.cc
    src/Core/Edge.cc
    src/Core/Random.cc
    src/Core/Building.cc
    src/Core/DevelopmentCard.cc
    src/Core/Resource.cc
    src/Core/Result.cc
    src/Core/Vertex.cc
    src/Game/BoardView/Hex.cc
    src/Game/BoardView/Junction.cc
    src/Game/BoardView/Node.cc
    src/Game/BoardView/Road.cc
    src/Game/Dice.cc
    src/Game/ExecutionOptions.cc
    src/Game/Game.cc
    src/Game/Player.cc
    src/Game/Trade.cc
    src/Scenario/Parameters.cc
    src/Scenario/Scenario.cc
    src/Server/Registrar.cc
    src/Server/Request.cc
    src/Server/Response.cc
    src/Server/Server.cc
    src/Util/Decoder.cc
    src/Util/Encoder.cc
    src/Util/ServerBase.cc
)
target_compile_options(k10 PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

link_directories(build)
link_libraries(k10)

add_executable(test-server src/Util/ServerBase_test.cc)
target_compile_options(test-server PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

add_executable(main src/main.cc)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

add_executable(test_
    src/test.cc
    src/Board/Graph_test.cc
    src/Board/Node_test.cc
    src/Board/Parser_test.cc
    src/Board/Port_test.cc
    src/Core/Action_test.cc
    src/Core/Resource_test.cc
    src/Game/Game_test.cc
    src/Scenario/Scenario_test.cc
    src/Server/Registrar_test.cc
    src/Server/Request_test.cc
    src/Server/Response_test.cc
    src/Util/Decoder_test.cc
    src/Util/DoublyLinkedList_test.cc
    src/Util/LinkedTrie_test.cc
    src/Util/LRUCache_test.cc
)
target_compile_options(test_ PRIVATE -Werror -g)
