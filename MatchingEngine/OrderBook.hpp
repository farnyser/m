#ifndef M_ORDERBOOK_HPP
#define M_ORDERBOOK_HPP

#include <set>
#include <iterator>
#include <vector>

#include <Core/InstrumentId.hpp>
#include <Core/Quantity.hpp>
#include <MatchingEngine/MatchingOrder.hpp>
#include <Core/Execution.hpp>

namespace M
{

	template <typename TOrder>
    class OrderBook
    {
	private:
		std::set<TimedOrder<TOrder>> buys;
		std::set<TimedOrder<TOrder>> sells;

	public:
		const InstrumentId instrument;
		unsigned int current{0};
	
		OrderBook(const InstrumentId id) : instrument(id) {}
	
		Execution Execute(const Order& order)
		{
			Execution result;
			size_t gc {0};
			Quantity remaining = order.quantity;

            auto& orders = order.direction == Direction::Buy ? sells : buys;
			auto signed_price = order.direction == Direction::Buy ? order.price : - order.price;

			for(auto& x : orders)
			{
				if (x.signed_price <= signed_price || order.type == Type::Market)
				{
					auto q = x.quantity < remaining ? x.quantity : remaining;
					result.quantity.push_back(q);
					result.price.push_back(x.price);
					const_cast<TimedOrder<TOrder>&>(x).quantity -= q;
					remaining -= q;
					gc += x.quantity == 0;

					if(remaining == 0)
						break;
				}
				else
				{
					break;
				}
			}

			// garbage collector
			if(gc > 0)
			{
				auto it = orders.begin();
				std::advance(it, gc);
				orders.erase(orders.begin(),it);
			}

			// insert remaining order part into book
			if(remaining > 0 && order.type != Type::Market)
            {
                if(order.direction == Direction::Buy)
                  buys.insert(TimedOrder<TOrder>{current++, remaining, order});
                else
                  sells.insert(TimedOrder<TOrder>{current++, remaining, order});
            }

			return result;
		}
		
		size_t Size() const 
		{
			return buys.size() + sells.size();
		}
    };
}

#endif //M_ORDERBOOK_HPP
