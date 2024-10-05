#pragma once

#include <Bitfinex/Forward.h>
#include <vector>
#include <ostream>

namespace Bitfinex {

class Positions {
public:
    Positions() = default;

    void append_position(Position const&);

    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::vector<Position> const& positions() const { return m_positions; }
private:
    std::vector<Position> m_positions;
};

std::ostream& operator<<(std::ostream& cin, Positions const& positions);

}