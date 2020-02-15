#include "Core/Result.h"

namespace k10engine::Game {

std::ostream& operator<<(std::ostream& os, const ResultType& error)
{
    switch (error) {
    case ResultType::Ok:
        os << "ResultType::Ok";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Result& result)
{
    os << "Result{ " << result.type;
    if (!result.args.empty()) {
        os << " { ";
        bool is_first = true;
        for (const auto& arg : result.args) {
            if (!is_first) {
                os << ", ";
            }
            os << arg;
            is_first = false;
        }
        os << " }";
    }
    os << " }";
    return os;
}

} // namespace k10engine::Game
