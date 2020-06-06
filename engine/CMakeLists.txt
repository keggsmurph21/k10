cmake_minimum_required(VERSION 3.0.2)
project(k10)

set(CMAKE_CXX_STANDARD 17)

find_program(CLANG_TIDY NAMES clang-tidy)
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})

include_directories(include)

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
    src/Scenario/Scenario.cc
    src/Server/Registrar.cc
    src/Server/Request.cc
    src/Server/Server.cc
    src/Util/ServerBase.cc
)
target_compile_options(k10 PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

link_directories(build)
link_libraries(k10)

add_executable(test-server src/Util/Test/ServerBase.cc)
target_compile_options(test-server PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

add_executable(main src/main.cc)
target_compile_options(main PRIVATE -Werror -Wall -Wextra -pedantic-errors -g)

add_executable(test_
    src/test.cc
    src/Board/Test/Graph.cc
    src/Board/Test/Node.cc
    src/Board/Test/Parser.cc
    src/Board/Test/Port.cc
    src/Core/Test/Resource.cc
    src/Game/Test/Game.cc
    src/Scenario/Test/Scenario.cc
    src/Server/Test/Registrar.cc
    src/Server/Test/Request.cc
    src/Util/Test/DoublyLinkedList.cc
    src/Util/Test/LinkedTrie.cc
    src/Util/Test/LRUCache.cc
    #src/Util/Test/Serializer.cc
    src/Util/Test/SerialStream.cc
)
target_compile_options(test_ PRIVATE -Werror -g)
