#include <Bitfinex/Positions.h>
#include <Bitfinex/Client.h>

namespace Bitfinex {

void Positions::append_position(Position const& position)
{
    m_positions.push_back(position);
}

bool Positions::empty() const
{
    return m_positions.empty();
}

std::ostream& operator<<(std::ostream& cin, Positions const& positions)
{
    std::for_each(positions.positions().cbegin(), positions.positions().cend(), [&cin](Position const& position) {
        cin << position << std::endl;
    });
    return cin;
}

}