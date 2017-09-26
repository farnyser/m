#ifndef M_PLACEUPDATECANCELTESTS_HPP
#define M_PLACEUPDATECANCELTESTS_HPP

#include <Core/InstrumentId.hpp>
#include <MatchingEngine/OrderBook.hpp>
#include <Core/Price.hpp>
#include <Core/Order.hpp>
#include <Core/Id.hpp>

#include <memory>
#include <map>

namespace M
{
	namespace Tests
	{
		namespace PlaceUpdateCancelTests
		{
			void should_cancel_pending_order()
			{
				auto book = M::Tests::BuildOrderBook(1);
				auto buy = WithId(Order::BuyLimit(1, Quantity{100}, Price{2000}));

				assertNotExecuted(book, buy);
				assertCanceled(book, buy);
			}

			void should_not_cancel_executed_order()
			{
				auto book = M::Tests::BuildOrderBook(1);
				auto buy = WithId(Order::BuyLimit(1, Quantity{100}, Price{2000}));
				auto sell = WithId(Order::Sell(1, Quantity{100}));

				assertNotExecuted(book, buy);
				assertExecutedAt(book, 2000, 100, sell);
				assertNotCanceled(book, buy);
			}
		};
	}
}

#endif //M_PLACEUPDATECANCELTESTS_HPP
