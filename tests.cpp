#include <iostream>
#include <Core/Order.hpp>
#include <MatchingEngine/OrderBook.hpp>
#include <Tests/Assert.hpp>

using namespace M;

auto BuildOrderBook(InstrumentId id)
{
	return Builder::OrderBook<Order>(id, [](Price p){});
}

void assertNotExecuted(Execution exec)
{
    assertEquals("Execution count", 0, exec.price.size());
    assertEquals("Execution count", 0, exec.quantity.size());
}

void assertExecutedAt(Price p, Quantity q, Execution exec)
{
    assertEquals("Execution count", 1, exec.price.size());
    assertEquals("Execution count", 1, exec.quantity.size());
    assertEquals("Price", p, exec.price[0]);
    assertEquals("Quantity", q, exec.quantity[0]);
}

void assertExecutedAt(Price p0, Quantity q0, Price p1, Quantity q1, Execution exec)
{
    assertEquals("Execution count", 2, exec.price.size());
    assertEquals("Execution count", 2, exec.quantity.size());
    assertEquals("1st Price", p0, exec.price[0]);
    assertEquals("1st Quantity", q0, exec.quantity[0]);
    assertEquals("2nd Price", p1, exec.price[1]);
    assertEquals("2nd Quantity", q1, exec.quantity[1]);
}

void simple_sell_buy_nomatch()
{
	auto book = BuildOrderBook(1);
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2001});
	auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{1999});
	
	assertNotExecuted(book.Execute(sell));
	assertNotExecuted(book.Execute(buy));
}

void simple_sell_buy_match()
{
	auto book = BuildOrderBook(1);
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	
	assertNotExecuted(book.Execute(sell));
	assertExecutedAt(2000, 10, book.Execute(buy));
}


void limit_sell_buy_match()
{
    auto book = BuildOrderBook(1);
    auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{1999});
    auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2001});

    assertNotExecuted(book.Execute(sell));
    assertExecutedAt(1999, 10, book.Execute(buy));
}

void simple_sell_limit_buy_market_match()
{
	auto book = BuildOrderBook(1);
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
	
	assertNotExecuted(book.Execute(sell));
	assertExecutedAt(2000, 10, book.Execute(buy));
}

void market_buy_rejected_when_no_seller()
{
	auto book = BuildOrderBook(1);
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
	
	assertNotExecuted(book.Execute(buy));
}

void market_buy_matching_two_sells()
{
	auto book = BuildOrderBook(1);
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2000});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2005});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});

	assertNotExecuted(book.Execute(sell1));
	assertNotExecuted(book.Execute(sell2));
	assertExecutedAt(2000, 3, 2005, 7, book.Execute(buy));
}

void market_buy_matching_two_sells_in_order()
{
	auto book = BuildOrderBook(1);
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2005});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2000});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
		
	assertNotExecuted(book.Execute(sell1));
	assertNotExecuted(book.Execute(sell2));
	assertExecutedAt(2000, 7, 2005, 3, book.Execute(buy));
}

void lot_of_order_behavior()
{
	auto book = BuildOrderBook(1);
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2005});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{200}, Price{1950});
	auto sell3 = Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2020});
	auto sell4 = Order::SellLimit(InstrumentId{1}, Quantity{500}, Price{2001});
	auto buy1 = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{1930});
	auto buy2 = Order::BuyLimit(InstrumentId{1}, Quantity{80}, Price{1945});
	auto buy3 = Order::BuyLimit(InstrumentId{1}, Quantity{700}, Price{1940});

    assertNotExecuted(book.Execute(sell1));
    assertNotExecuted(book.Execute(sell2));
    assertNotExecuted(book.Execute(sell3));
    assertNotExecuted(book.Execute(sell4));
    assertNotExecuted(book.Execute(buy1));
    assertNotExecuted(book.Execute(buy2));
    assertNotExecuted(book.Execute(buy3));

	assertEquals(7, book.Size());

    assertExecutedAt(1945, 50, book.Execute(Order::Sell(InstrumentId{1}, Quantity{50})));
}

void remain_partial_then_exec()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book.Execute(Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000})));
	assertEquals(1, book.Size());

	assertExecutedAt(2000, 100, book.Execute(Order::BuyLimit(InstrumentId{1}, Quantity{150}, Price{2000})));
	assertEquals(1, book.Size());

	assertExecutedAt(2000, 50, book.Execute(Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{1990})));
	assertEquals(0, book.Size());
}

int main()
{
	test("simple_sell_buy_match", simple_sell_buy_match);
	test("simple_sell_buy_nomatch", simple_sell_buy_nomatch);
	test("simple_sell_limit_buy_market_match", simple_sell_limit_buy_market_match);
	test("market_buy_matching_two_sells", market_buy_matching_two_sells);
	test("market_buy_matching_two_sells_in_order", market_buy_matching_two_sells_in_order);
	test("market_buy_rejected_when_no_seller", market_buy_rejected_when_no_seller);
	test("limit_sell_buy_match", limit_sell_buy_match);
	test("lot_of_order_behavior", lot_of_order_behavior);
	test("remain_partial_then_exec", remain_partial_then_exec);

    return 0;
}