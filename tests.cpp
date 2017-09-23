#include <iostream>
#include <Core/Order.hpp>
#include <Core/OrderBook.hpp>
#include <Tests/Assert.hpp>

using namespace M;

void assertExecutedAt(Price p, Quantity q, Execution exec)
{
    assertEquals(1, exec.price.size());
    assertEquals(1, exec.quantity.size());
    assertEquals(p, exec.price[0]);
    assertEquals(q, exec.quantity[0]);
}

void simple_sell_buy_nomatch()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2001});
	auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{1999});
	
	auto sellExec = book.Execute(sell);
	auto buyExec = book.Execute(buy);
	
	assertEquals(0, sellExec.quantity.size());
	assertEquals(0, buyExec.quantity.size());
}

void simple_sell_buy_match()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	
	auto sellExec = book.Execute(sell);
	auto buyExec = book.Execute(buy);

	assertEquals(0, sellExec.quantity.size());
	assertEquals(10, buyExec.quantity[0]);
	assertEquals(2000, buyExec.price[0]);
}


void limit_sell_buy_match()
{
    auto book = OrderBook{InstrumentId{1}};
    auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{1999});
    auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2001});

    auto sellExec = book.Execute(sell);
    auto buyExec = book.Execute(buy);

    assertEquals(0, sellExec.quantity.size());
    assertEquals(1, buyExec.quantity.size());
    assertEquals(10, buyExec.quantity[0]);
    assertEquals(1999, buyExec.price[0]);
}

void simple_sell_limit_buy_market_match()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
	
	auto sellExec = book.Execute(sell);
	auto buyExec = book.Execute(buy);

	assertEquals(0, sellExec.quantity.size());
	assertEquals(10, buyExec.quantity[0]);
	assertEquals(2000, buyExec.price[0]);
}

void market_buy_rejected_when_no_seller()
{
	auto book = OrderBook{InstrumentId{1}};
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
	
	auto buyExec = book.Execute(buy);

	assertEquals(0, buyExec.quantity.size());
	assertEquals(0, book.Size());
}

void simple_buy_matching_two_sells()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2000});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2000});
	auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2000});
		
	auto sellExec1 = book.Execute(sell1);
	auto sellExec2 = book.Execute(sell2);
	auto buyExec = book.Execute(buy);

	assertEquals(0, sellExec1.quantity.size());
	assertEquals(0, sellExec2.quantity.size());
	assertEquals(2, buyExec.quantity.size());
	assertEquals(3, buyExec.quantity[0]);
	assertEquals(2000, buyExec.price[0]);
	assertEquals(7, buyExec.quantity[1]);
	assertEquals(2000, buyExec.price[1]);
}

void market_buy_matching_two_sells()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2000});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2005});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
		
	auto sellExec1 = book.Execute(sell1);
	auto sellExec2 = book.Execute(sell2);
	auto buyExec = book.Execute(buy);

	assertEquals(0, sellExec1.quantity.size());
	assertEquals(0, sellExec2.quantity.size());
	assertEquals(2, buyExec.quantity.size());
	assertEquals(3, buyExec.quantity[0]);
	assertEquals(2000, buyExec.price[0]);
	assertEquals(7, buyExec.quantity[1]);
	assertEquals(2005, buyExec.price[1]);
}

void market_buy_matching_two_sells_in_order()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2005});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2000});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
		
	auto sellExec1 = book.Execute(sell1);
	auto sellExec2 = book.Execute(sell2);
	auto buyExec = book.Execute(buy);

	assertEquals(0, sellExec1.quantity.size());
	assertEquals(0, sellExec2.quantity.size());
	assertEquals(2, buyExec.quantity.size());
	assertEquals(7, buyExec.quantity[0]);
	assertEquals(2000, buyExec.price[0]);
	assertEquals(3, buyExec.quantity[1]);
	assertEquals(2005, buyExec.price[1]);
}

void lot_of_order_behavior()
{
	auto book = OrderBook{InstrumentId{1}};
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2005});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{200}, Price{1950});
	auto sell3 = Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2020});
	auto sell4 = Order::SellLimit(InstrumentId{1}, Quantity{500}, Price{2001});
	auto buy1 = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{1930});
	auto buy2 = Order::BuyLimit(InstrumentId{1}, Quantity{80}, Price{1945});
	auto buy3 = Order::BuyLimit(InstrumentId{1}, Quantity{700}, Price{1940});

    book.Execute(sell1);
    book.Execute(sell2);
    book.Execute(sell3);
    book.Execute(sell4);
    book.Execute(buy1);
    book.Execute(buy2);
    book.Execute(buy3);

	assertEquals(7, book.Size());

    assertExecutedAt(1945, 50, book.Execute(Order::Sell(InstrumentId{1}, Quantity{50})));
}

int main()
{
	test("simple_sell_buy_match", simple_sell_buy_match);
	test("simple_sell_buy_nomatch", simple_sell_buy_nomatch);
	test("simple_buy_matching_two_sells", simple_buy_matching_two_sells);
	test("simple_sell_limit_buy_market_match", simple_sell_limit_buy_market_match);
	test("market_buy_matching_two_sells", market_buy_matching_two_sells);
	test("market_buy_matching_two_sells_in_order", market_buy_matching_two_sells_in_order);
	test("market_buy_rejected_when_no_seller", market_buy_rejected_when_no_seller);
	test("limit_sell_buy_match", limit_sell_buy_match);
	test("lot_of_order_behavior", lot_of_order_behavior);

    return 0;
}