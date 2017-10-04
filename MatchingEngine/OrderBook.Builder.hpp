#ifndef M_ORDERBOOK_BUILDER_HPP
#define M_ORDERBOOK_BUILDER_HPP

#include <MatchingEngine/MatchingOrder.hpp>
#include <Core/OrderStatus.hpp>
#include "Helper.hpp"

namespace M
{
	namespace Builder
	{
		void VoidPriceCallback(Price) {}

		template<typename TOrder>
		void VoidExecutionCallback(const TOrder &, Price, M::Quantity) {}

		template<typename TId>
		void VoidOrderLifecycleCallback(const TId &, M::OrderStatus) {}

		template<typename TOrder, typename TPriceCallback, typename TExecutionCallback, typename TLifeCycleCallback>
		auto OrderBook(M::InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback,
					   TLifeCycleCallback orderLifeCycleCallback)
		{
			return M::OrderBook<TOrder, TPriceCallback, TExecutionCallback, TLifeCycleCallback>(id, priceCallback,
																								executionCallback,
																								orderLifeCycleCallback);
		};

		template<typename TOrder, typename TPriceCallback, typename TExecutionCallback>
		auto OrderBook(M::InstrumentId id, TPriceCallback priceCallback, TExecutionCallback executionCallback)
		{
			return Builder::OrderBook<TOrder>(id, priceCallback, executionCallback,
											  VoidOrderLifecycleCallback<decltype(TOrder::identifier)>);
		};

		template<typename TOrder, typename TPriceCallback>
		auto OrderBook(M::InstrumentId id, TPriceCallback priceCallback)
		{
			return Builder::OrderBook<TOrder>(id, priceCallback, VoidExecutionCallback<TOrder>,
											  VoidOrderLifecycleCallback<decltype(TOrder::identifier)>);
		};

		template<typename TOrder>
		auto OrderBook(M::InstrumentId id)
		{
			return Builder::OrderBook<TOrder>(id, VoidPriceCallback, VoidExecutionCallback<TOrder>,
											  VoidOrderLifecycleCallback<decltype(TOrder::identifier)>);
		};
	}
}

#endif //M_ORDERBOOK_BUILDER_HPP
