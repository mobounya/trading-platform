#pragma once

#include <Bitfinex/Forward.h>
#include <vector>
#include <ostream>

namespace Bitfinex {

class OrderBook {
public:
    OrderBook() = default;

    void append_order(Order const&);

    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::vector<Order> const& order_book() const { return m_order_book; }
private:
    std::vector<Order> m_order_book;
};

std::ostream& operator<<(std::ostream& cin, OrderBook const&);
}
