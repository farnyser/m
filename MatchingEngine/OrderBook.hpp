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
	template <typename TOrder, typename TPriceCallback, typename TExecutionCallback>
    class OrderBook
    {
	private:
		std::set<MatchingOrder<TOrder>> buys;
		std::set<MatchingOrder<TOrder>> sells;
        TPriceCallback priceCallback;
		TExecutionCallback executionCallback;

		bool EnsureAvailability(const std::set<MatchingOrder<TOrder>>& orders, Quantity quantity, unsigned int signed_price, Type type)
		{
			for(auto& x : orders)
			{
				if(x.signed_price > signed_price && type != Type::Market)
					return false;

				if(quantity > x.quantity)
					quantity -= x.quantity;
				else
					return true;
			}

			return false;
		}

		void InsertInBook(const TOrder& order, Quantity remaining)
		{
			if(remaining == 0 || order.type == Type::Market || order.timeInForce == TimeInForce::ImmediateOrCancel)
				return;

			if(order.direction == Direction::Buy)
				buys.insert(MatchingOrder<TOrder>{current++, remaining, order});
			else
				sells.insert(MatchingOrder<TOrder>{current++, remaining, order});
		}

	public:
		const InstrumentId instrument;
		unsigned int current{0};
	
		OrderBook(const InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback)
                : instrument(id), priceCallback(priceCallback), executionCallback(executionCallback)
        {
        }
	
		Execution Execute(const Order& order)
		{
			Execution result;
			size_t gc {0};
			Quantity remaining = order.quantity;

            auto& orders = order.direction == Direction::Buy ? sells : buys;
			auto signed_price = order.direction == Direction::Buy ? order.price : - order.price;

			if(order.fulfillment == Fulfillment::Full && !EnsureAvailability(orders, order.quantity, signed_price, order.type))
			{
				InsertInBook(order, order.quantity);
				return result;
			}

			for(auto& x : orders)
			{
				if (x.signed_price <= signed_price || order.type == Type::Market)
				{
					auto q = x.quantity < remaining ? x.quantity : remaining;
					result.quantity.push_back(q);
					result.price.push_back(x.price);
					const_cast<MatchingOrder<TOrder>&>(x).quantity -= q;
					remaining -= q;
					gc += x.quantity == 0;
					executionCallback(x, q, x.price);
					executionCallback(order, q, x.price);
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
			InsertInBook(order, remaining);

            if(result.price.size())
                priceCallback(*result.price.rbegin());

			return result;
		}
		
		size_t Size() const 
		{
			return buys.size() + sells.size();
		}
    };

	namespace Builder
	{
		void VoidPriceCallback(Price) {}

		template <typename TOrder>
		void VoidExecutionCallback(const TOrder&, Price, Quantity) {}

		template <typename TOrder>
		M::OrderBook<TOrder, void(*)(Price), void(*)(const TOrder&, Price, Quantity)> OrderBook(M::InstrumentId id)
		{
			return M::OrderBook<TOrder, void(*)(Price), void(*)(const TOrder&, Price, Quantity)>(id, VoidPriceCallback, VoidExecutionCallback<TOrder>);
		};

		template <typename TOrder, typename TPriceCallback>
		M::OrderBook<TOrder, TPriceCallback, void(*)(const TOrder& o, Price p, Quantity q)> OrderBook(M::InstrumentId id, TPriceCallback priceCallback)
		{
			return M::OrderBook<TOrder, TPriceCallback, void(*)(const TOrder&, Price, Quantity)>(id, priceCallback, VoidExecutionCallback<TOrder>);
		};

		template <typename TOrder, typename TPriceCallback, typename TExecutionCallback>
		M::OrderBook<TOrder, TPriceCallback, TExecutionCallback> OrderBook(M::InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback)
		{
			return M::OrderBook<TOrder, TPriceCallback, TExecutionCallback>(id, priceCallback, executionCallback);
		};
	}
}

#endif //M_ORDERBOOK_HPP
