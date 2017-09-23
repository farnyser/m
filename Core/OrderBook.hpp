#ifndef M_ORDERBOOK_HPP
#define M_ORDERBOOK_HPP

#include <set>
#include <vector>

#include <Core/InstrumentId.hpp>
#include <Core/Quantity.hpp>

namespace M
{
	struct Execution
	{
		const std::vector<Quantity> quantity;
		const std::vector<Price> price;
	};
	
	struct TimedOrder : Order 
	{
		const unsigned int time;
        const signed int signed_price;

		TimedOrder(unsigned int time, Order order)
		: Order(order.instrument, order.quantity, order.price, order.direction, order.type), time(time), signed_price(order.price * (order.direction == Direction::Buy ? -1 : 1))
		{
		}
		
		bool operator<(const TimedOrder& other) const
		{
			if(type == Type::Market && other.type != Type::Market)
				return true;
			if(type != Type::Market && other.type == Type::Market)
				return false;

			if(signed_price < other.signed_price)
				return true;
			if(signed_price > other.signed_price)
				return false;

			if(time < other.time)
				return true;
			if(time > other.time)
				return false;

			return false;
		}
	};
	
    class OrderBook
    {
	private:
		std::set<TimedOrder> buys;
		std::set<TimedOrder> sells;

	public:
		const InstrumentId instrument;
		unsigned int current{0};
	
		OrderBook(const InstrumentId id) : instrument(id) {}
	
		Execution Execute(const Order& order)
		{
			std::vector<Quantity> q;
			std::vector<Price> p;

            auto& orders = order.direction == Direction::Buy ? sells : buys;
            auto cmp = order.direction == Direction::Buy ?
                       [](Price b, Price o) { return b <= o; }
                       : [](Price b, Price o) { return b >= o; };
			
			for(auto& x : orders)
			{
				if (x.direction != order.direction && (cmp(x.price,order.price) || order.type == Type::Market))
				{
					if(x.quantity >= order.quantity)
					{
						q.push_back(order.quantity);
						p.push_back(x.price);
						const_cast<TimedOrder&>(x).quantity -= order.quantity;
						break;
					}
					else  
					{
						q.push_back(x.quantity);						
						p.push_back(x.price);
						const_cast<TimedOrder&>(x).quantity = 0;
					}					
				}
			}
			
			if(order.quantity != 0 && order.type != Type::Market)
            {
                if(order.direction == Direction::Buy)
                  buys.insert(TimedOrder{current++, order});
                else
                  sells.insert(TimedOrder{current++, order});
            }

			return Execution{q,p};
		}
		
		size_t Size() const 
		{
			return buys.size() + sells.size();
		}
    };
}

#endif //M_ORDERBOOK_HPP
