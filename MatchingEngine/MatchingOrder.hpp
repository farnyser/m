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
                : TOrder(order),
                  time(time),
                  signed_price(order.price * (order.direction == Direction::Buy ? -1 : 1))
		{
			this->quantity = quantity;
        }

        bool operator<(const MatchingOrder<TOrder> &other) const
        {
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
