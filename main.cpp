#include <iostream>
#include <Core/Order.hpp>
#include <Core/OrderBook.hpp>
#include <Tests/Assert.hpp>

int main()
{
    using namespace M;
	auto book = OrderBook{InstrumentId{1}};
	auto execCount = size_t{0};

	test("simulation", [&]()
	{
		for (auto i = 0; i < 10'000'000; i++)
		{
			auto order = Order
			{
				InstrumentId{1},
				Quantity{10 * (i % 10)},
				Price{1975 + i % 50 + (i % 500) / 10},
				i % 10 > 5 ? Direction::Buy : Direction::Sell,
				i % 100 < 10 ? Type::Market : Type::Limit,
			};

			execCount += book.Execute(order).quantity.size();
		}
	});

	std::cout << "Number of executions: " << execCount << std::endl;
	std::cout << "Remaining in book: " << book.Size() << std::endl;
    return 0;
}