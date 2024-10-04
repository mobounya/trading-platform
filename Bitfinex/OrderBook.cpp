#include <Bitfinex/OrderBook.h>
#include <Bitfinex/Client.h>

namespace Bitfinex {

void OrderBook::append_order(Bitfinex::Order const& order)
{
    m_order_book.push_back(order);
}

size_t OrderBook::empty() const
{
    return m_order_book.empty();
}

std::ostream& operator<<(std::ostream& cin, OrderBook const& order_book)
{
    std::for_each(order_book.order_book().begin(), order_book.order_book().end(), [&cin](Order const& order) {
       cin << order << std::endl;
    });
    return cin;
}

}