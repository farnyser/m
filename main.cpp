#include <iostream>
#include <Core/Order.hpp>
#include <MatchingEngine/OrderBook.hpp>
#include <Tests/Assert.hpp>
#include <Core/Id.hpp>

int main()
{
    using namespace M;
	auto priceCallback = [](Price p){};
	auto book = Builder::OrderBook<Id<Order>>(InstrumentId{1}, priceCallback);
	auto execCount = size_t{0};

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
			execCount += book.Execute(identifiedOrder).quantity.size();
		}
	});

	std::cout << "Number of executions: " << execCount << std::endl;
	std::cout << "Remaining in book: " << book.Size() << std::endl;
    return 0;
}