#pragma once

#include <iostream>
#include <string.h>
#include <vector>

#include "Core/Edge.h"
#include "Util/ByteBuffer.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

/**
 * Examples:
 *
 * // Build a settlement at Node(42)
 * { State::Edge::Build,
 *   { { ActionArgumentType::BuildItemId, Building::Settlement },
 *     { ActionArgumentType::NodeId, 42 } } };
 *
 * // Offer to trade 1 Brick for 2 Sheep
 * { State::Edge::OfferTrade,
 *   { { ActionArgumentType::GiveResourceType, Resource::Brick },
 *     { ActionArgumentType::ResourceCount, 1 },
 *     { ActionArgumentType::TakeResourceType, Resource::Sheep },
 *     { ActionArgumentType::ResourceCount, 2 } } };
 */

namespace k10engine::Game {

enum class ActionArgumentType {
    NodeId,
    DiceRoll,
    PlayerId,
    BuildItemId,
    DevelopmentCardId,
    TakeResourceType,
    GiveResourceType,
    ResourceCount,
};

struct ActionArgument {
    ActionArgumentType type;
    size_t value;
};

struct Action {
    State::Edge edge;
    std::vector<ActionArgument> args;
};

std::ostream& operator<<(std::ostream&, const ActionArgumentType&);
std::ostream& operator<<(std::ostream&, const ActionArgument&);
std::ostream& operator<<(std::ostream&, const Action&);

} // namespace k10engine::Game

template<>
void encode(ByteBuffer&, const k10engine::Game::ActionArgumentType&);

template<>
bool decode(ByteBuffer&, k10engine::Game::ActionArgumentType&);

template<>
void encode(ByteBuffer&, const k10engine::Game::ActionArgument&);

template<>
bool decode(ByteBuffer&, k10engine::Game::ActionArgument&);

template<>
void encode(ByteBuffer&, const k10engine::Game::Action&);

template<>
bool decode(ByteBuffer&, k10engine::Game::Action&);
