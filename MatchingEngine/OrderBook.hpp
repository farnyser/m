#ifndef M_ORDERBOOK_HPP
#define M_ORDERBOOK_HPP

#include <set>
#include <iterator>
#include <vector>

#include <Core/InstrumentId.hpp>
#include <Core/Quantity.hpp>
#include <MatchingEngine/MatchingOrder.hpp>
#include <Core/OrderStatus.hpp>
#include "Helper.hpp"

namespace M
{
	template <typename TOrder, typename TPriceCallback, typename TExecutionCallback, typename TOrderLifecycleCallback>
    class OrderBook
    {
	private:
		std::set<MatchingOrder<TOrder>> buys;
		std::set<MatchingOrder<TOrder>> sells;
        TPriceCallback priceCallback;
		TExecutionCallback executionCallback;
		TOrderLifecycleCallback orderLifecycleCallback;

		void InsertInBook(const TOrder& order, Quantity remaining)
		{
			if(remaining == 0)
				return;

			if(order.type == Type::Market || order.timeInForce == TimeInForce::ImmediateOrCancel)
				return orderLifecycleCallback(order, OrderStatus::Canceled);

			if(order.direction == Direction::Buy)
				buys.insert(MatchingOrder<TOrder>{current++, remaining, order});
			else
				sells.insert(MatchingOrder<TOrder>{current++, remaining, order});

			return orderLifecycleCallback(order, OrderStatus::Placed);
		}

	public:
		const InstrumentId instrument;
		unsigned int current{0};
	
		OrderBook(const InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback, TOrderLifecycleCallback orderLifecycleCallback)
                : instrument(id), priceCallback(priceCallback), executionCallback(executionCallback), orderLifecycleCallback(orderLifecycleCallback)
        {
        }
	
		void Execute(const TOrder& order)
		{
			size_t gc {0};
			Quantity remaining = order.quantity;

            auto& orders = order.direction == Direction::Buy ? sells : buys;
			auto signed_price = order.direction == Direction::Buy ? order.price : - order.price;

			if(order.fulfillment == Fulfillment::Full && !CheckExecutability(orders, order.quantity, signed_price, order.type))
				return InsertInBook(order, order.quantity);

			auto last_price = Price{0};
			for(auto& x : orders)
			{
				if (x.signed_price <= signed_price || order.type == Type::Market)
				{
					auto q = x.quantity < remaining ? x.quantity : remaining;
					const_cast<MatchingOrder<TOrder>&>(x).quantity -= q;
					remaining -= q;
					gc += x.quantity == 0;
					executionCallback(x, x.price, q);
					executionCallback(order, x.price, q);
					last_price = x.price;
					if(remaining == 0)
						break;
				}
				else
				{
					break;
				}
			}

			if(last_price)
				priceCallback(last_price);

			// garbage collector
			if(gc > 0)
			{
				auto it = orders.begin();
				std::advance(it, gc);
				orders.erase(orders.begin(),it);
			}

			// insert remaining order part into book
			InsertInBook(order, remaining);
		}

		template <typename TId>
		bool Cancel(const TId& id)
		{
			for(auto i = 0; i < 2; i++)
			{
				auto& orders = i == 0 ? buys : sells;
				for(auto it = orders.begin(); it != orders.end(); ++it)
				{
					if(it->identifier == id)
					{
						orders.erase(it);
						return true;
					}
				}
			}
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
		void VoidOrderLifecycleCallback(const TOrder&, OrderStatus) {}

		template <typename TOrder, typename TPriceCallback, typename TExecutionCallback, typename TLifeCycleCallback>
		auto OrderBook(M::InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback, TLifeCycleCallback orderLifeCycleCallback)
		{
			return M::OrderBook<TOrder, TPriceCallback, TExecutionCallback, TLifeCycleCallback>(id, priceCallback, executionCallback, orderLifeCycleCallback);
		};

		template <typename TOrder, typename TPriceCallback, typename TExecutionCallback>
		auto OrderBook(M::InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback)
		{
			return Builder::OrderBook<TOrder>(id, priceCallback, executionCallback, VoidOrderLifecycleCallback<TOrder>);
		};

		template <typename TOrder, typename TPriceCallback>
		auto OrderBook(M::InstrumentId id, TPriceCallback priceCallback)
		{
			return Builder::OrderBook<TOrder>(id, priceCallback, VoidExecutionCallback<TOrder>, VoidOrderLifecycleCallback<TOrder>);
		};

		template <typename TOrder>
		auto OrderBook(M::InstrumentId id)
		{
			return Builder::OrderBook<TOrder>(id, VoidPriceCallback, VoidExecutionCallback<TOrder>, VoidOrderLifecycleCallback<TOrder>);
		};
	}
}

#endif //M_ORDERBOOK_HPP
