#pragma once

// Put all of our class declarations here so that
// we can avoid polluting our actual headers with
// forward declarations :^)

namespace k10engine {

class Random;

namespace Graph {

class Graph;
class Node;
class Port;

} // namespace Graph

namespace Game {

class Dice;
class Game;
class Player;
class Robber;

namespace BoardView {

class Hex;
class Junction;
class NodeView;
class Road;

} // namespace BoardView

} // namespace Game

namespace Scenario {

class Scenario;

} // namespace Scenario

} // namespace k10engine
