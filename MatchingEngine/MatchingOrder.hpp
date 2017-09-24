#ifndef M_MATCHINGORDER_HPP
#define M_MATCHINGORDER_HPP

namespace M
{
    template<typename TOrder>
    struct MatchingOrder : TOrder
    {
        const unsigned int time;
        const signed int signed_price;

        MatchingOrder(unsigned int time, Quantity quantity, TOrder order)
                : TOrder(order.instrument, quantity, order.price, order.direction, order.type, order.fulfillment),
                  time(time),
                  signed_price(order.price * (order.direction == Direction::Buy ? -1 : 1)) {
        }

        bool operator<(const MatchingOrder<TOrder> &other) const
        {
            if (this->type == Type::Market && other.type != Type::Market)
                return true;
            if (this->type != Type::Market && other.type == Type::Market)
                return false;

            if (signed_price < other.signed_price)
                return true;
            if (signed_price > other.signed_price)
                return false;

            if (time < other.time)
                return true;
            if (time > other.time)
                return false;

            return false;
        }
    };
}

#endif //M_MATCHINGORDER_HPP
