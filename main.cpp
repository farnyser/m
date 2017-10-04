#include <iostream>
#include <Core/Order.hpp>
#include <Core/Id.hpp>
#include <MatchingEngine/OrderBook.hpp>
#include <MatchingEngine/OrderBook.Builder.hpp>
#include <Tests/Assert.hpp>

int main()
{
    using namespace M;
	auto priceCallback = [](Price p){};
	auto execCount = size_t{0};
	auto book = Builder::OrderBook<Id<Order>>(InstrumentId{1}, priceCallback, [&](const Id<Order>& order, Price p, Quantity q){ execCount++; });

	test("simulation", [&]()
	{
		for (auto i = 0; i < 10'000'000; i++)
		{
			auto order = Order
			{
				InstrumentId{1},
				static_cast<Quantity>(10 * (i % 10)),
                static_cast<Price>(1975 + i % 50 + (i % 500) / 10),
				i % 10 > 5 ? Direction::Buy : Direction::Sell,
				i % 100 < 10 ? Type::Market : Type::Limit,
				i % 2 ? Fulfillment::Partial : Fulfillment::Full,
				i % 10 < 5  ? TimeInForce::ImmediateOrCancel : TimeInForce::Day,
			};

			auto identifiedOrder = Id<Order>(i, order);
			book.Execute(identifiedOrder);
		}
	});

	std::cout << "Number of executions: " << execCount << std::endl;
	std::cout << "Remaining in book: " << book.Size() << std::endl;
    return 0;
}