#include <iostream>
#include <Core/Order.hpp>
#include <Core/OrderBook.hpp>

int main()
{
    using namespace M;

	auto book = OrderBook{InstrumentId{1}};
	
	for(auto i = 0; i < 10'000'000; i++)
	{
		auto order = Order	{
								InstrumentId{1}, 
								Quantity{10 * i % 10}, 
								Price{1975 + i % 50 + (i % 500)/10},
								i%10 > 5 ? Direction::Buy : Direction::Sell, 
								i%100 < 10 ? Type::Market : Type::Limit,
							};
							
		auto result = book.Execute(order);
    }
		
	std::cout << "Hello, World !" << std::endl;
    return 0;
}