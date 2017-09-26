#ifndef M_ASSERTEXTENSION_HPP
#define M_ASSERTEXTENSION_HPP

#include <map>
#include <memory>
#include <Core/Id.hpp>

namespace M
{
	namespace Tests
	{
		struct OrderExecution
		{
			std::vector<Price> price;
			std::vector<Quantity> quantity;
		};

		template <typename TOrder>
		Id<TOrder> WithId(const TOrder& order)
		{
			static int i = 0;
			return Id<TOrder>(i++, order);
		}

		template <typename TOrder>
		Id<TOrder> WithId(const Id<TOrder>& order)
		{
			return order;
		}

		auto BuildOrderBook(InstrumentId id)
		{
			auto execs = std::shared_ptr<std::map<int, OrderExecution>>(new std::map<int, OrderExecution>());

			return std::make_pair(Builder::OrderBook < Id < Order >> (id, [](Price p) {}, [=](const Id <Order> &order, Price p, Quantity q)
			{
				(*execs)[order.identifier].price.push_back(p);
				(*execs)[order.identifier].quantity.push_back(q);
			}), execs);
		}

		template <typename TBook, typename TOrder>
		void assertNotExecuted(TBook& book, const TOrder& o)
		{
			auto order = WithId(o);
			book.first.Execute(order);
			auto exec = (*book.second)[order.identifier];
			assertEquals("Execution count", 0, exec.price.size());
			assertEquals("Execution count", 0, exec.quantity.size());
		}

		template <typename TBook, typename TOrder>
		void assertCanceled(TBook& book, const TOrder& o)
		{
			auto initialSize = book.first.Size();
			auto order = WithId(o);
			book.first.Cancel(order.identifier);
			auto finalSize = book.first.Size();

			assertEquals("Size", initialSize - 1, finalSize);
		}

		template <typename TBook, typename TOrder>
		void assertNotCanceled(TBook& book, const TOrder& o)
		{
			auto initialSize = book.first.Size();
			auto order = WithId(o);
			book.first.Cancel(order.identifier);
			auto finalSize = book.first.Size();

			assertEquals("Size", initialSize, finalSize);
		}

		template <typename TBook, typename TOrder>
		void assertExecutedAt(TBook& book, Price p, Quantity q, const TOrder& o)
		{
			auto order = WithId(o);
			book.first.Execute(order);
			auto exec = (*book.second)[order.identifier];
			assertEquals("Execution count", 1, exec.price.size());
			assertEquals("Execution count", 1, exec.quantity.size());
			assertEquals("Price", p, exec.price[0]);
			assertEquals("Quantity", q, exec.quantity[0]);
		}

		template <typename TBook, typename TOrder>
		void assertExecutedAt(TBook& book, Price p0, Quantity q0, Price p1, Quantity q1, const TOrder& o)
		{
			auto order = WithId(o);
			book.first.Execute(order);
			auto exec = (*book.second)[order.identifier];
			assertEquals("Execution count", 2, exec.price.size());
			assertEquals("Execution count", 2, exec.quantity.size());
			assertEquals("1st Price", p0, exec.price[0]);
			assertEquals("1st Quantity", q0, exec.quantity[0]);
			assertEquals("2nd Price", p1, exec.price[1]);
			assertEquals("2nd Quantity", q1, exec.quantity[1]);
		}
	}
}

#endif //M_ASSERTEXTENSION_HPP
