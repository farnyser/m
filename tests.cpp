#include <iostream>
#include <Core/Order.hpp>
#include <MatchingEngine/OrderBook.hpp>
#include <Tests/Assert.hpp>

using namespace M;

auto BuildOrderBook(InstrumentId id)
{
	return Builder::OrderBook<Order>(id, [](Price p){}, [](const Order& order, Price p, Quantity q){
//		std::cout << "Order executed at " << p << " (quantity: " << q << ")" << std::endl;
	});
}

template <typename TBook, typename TOrder>
void assertNotExecuted(TBook& book, const TOrder& order)
{
	auto exec = book.Execute(order);
    assertEquals("Execution count", 0, exec.price.size());
    assertEquals("Execution count", 0, exec.quantity.size());
}

template <typename TBook, typename TOrder>
void assertExecutedAt(TBook& book, Price p, Quantity q, const TOrder& order)
{
	auto exec = book.Execute(order);
	assertEquals("Execution count", 1, exec.price.size());
    assertEquals("Execution count", 1, exec.quantity.size());
    assertEquals("Price", p, exec.price[0]);
    assertEquals("Quantity", q, exec.quantity[0]);
}

template <typename TBook, typename TOrder>
void assertExecutedAt(TBook& book, Price p0, Quantity q0, Price p1, Quantity q1, const TOrder& order)
{
	auto exec = book.Execute(order);
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
	
	assertNotExecuted(book, sell);
	assertNotExecuted(book, buy);
}

void simple_sell_buy_match()
{
	auto book = BuildOrderBook(1);
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	
	assertNotExecuted(book, sell);
	assertExecutedAt(book, 2000, 10, (buy));
}


void limit_sell_buy_match()
{
    auto book = BuildOrderBook(1);
    auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{1999});
    auto buy = Order::BuyLimit(InstrumentId{1}, Quantity{10}, Price{2001});

    assertNotExecuted(book, sell);
    assertExecutedAt(book, 1999, 10, (buy));
}

void simple_sell_limit_buy_market_match()
{
	auto book = BuildOrderBook(1);
	auto sell = Order::SellLimit(InstrumentId{1}, Quantity{10}, Price{2000});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
	
	assertNotExecuted(book, sell);
	assertExecutedAt(book, 2000, 10, (buy));
}

void market_buy_rejected_when_no_seller()
{
	auto book = BuildOrderBook(1);
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
	
	assertNotExecuted(book, buy);
}

void market_buy_matching_two_sells()
{
	auto book = BuildOrderBook(1);
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2000});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2005});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});

	assertNotExecuted(book, sell1);
	assertNotExecuted(book, sell2);
	assertExecutedAt(book, 2000, 3, 2005, 7, (buy));
}

void market_buy_matching_two_sells_in_order()
{
	auto book = BuildOrderBook(1);
	auto sell1 = Order::SellLimit(InstrumentId{1}, Quantity{3}, Price{2005});
	auto sell2 = Order::SellLimit(InstrumentId{1}, Quantity{7}, Price{2000});
	auto buy = Order::Buy(InstrumentId{1}, Quantity{10});
		
	assertNotExecuted(book, sell1);
	assertNotExecuted(book, sell2);
	assertExecutedAt(book, 2000, 7, 2005, 3, (buy));
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

    assertNotExecuted(book, sell1);
    assertNotExecuted(book, sell2);
    assertNotExecuted(book, sell3);
    assertNotExecuted(book, sell4);
    assertNotExecuted(book, buy1);
    assertNotExecuted(book, buy2);
    assertNotExecuted(book, buy3);

	assertEquals(7, book.Size());

    assertExecutedAt(book, 1945, 50, (Order::Sell(InstrumentId{1}, Quantity{50})));
}

void remain_partial_then_exec()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertEquals(1, book.Size());

	assertExecutedAt(book, 2000, 100, (Order::BuyLimit(InstrumentId{1}, Quantity{150}, Price{2000})));
	assertEquals(1, book.Size());

	assertExecutedAt(book, 2000, 50, (Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{1990})));
	assertEquals(0, book.Size());
}

void partial_market_order_should_be_filled_partially_if_not_enough_available()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2002}));
	assertExecutedAt(book, 2000, 100, 2002, 50, (Order::Buy(InstrumentId{1}, Quantity{200}).Partial()));
	assertEquals(0, book.Size());
}

void all_or_nothing_market_order_should_be_canceled_if_not_enough_available()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2002}));
	assertNotExecuted(book, Order::Buy(InstrumentId{1}, Quantity{200}).FullOrNothing());
	assertEquals(2, book.Size());
}

void all_or_nothing_market_order_should_be_executed_if_enough_quantity()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{150}, Price{2002}));
	assertExecutedAt(book, 2000, 100, 2002, 100, (Order::Buy(InstrumentId{1}, Quantity{200}).FullOrNothing()));
	assertEquals(1, book.Size());
}

void partial_limit_order_should_be_filled_partially_if_not_enough_available()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2002}));
	assertExecutedAt(book, 2000, 100, (Order::BuyLimit(InstrumentId{1}, Quantity{200}, Price{2001}).Partial()));
	assertEquals(2, book.Size());
}

void all_or_nothing_limit_order_should_be_inserted_in_book_if_not_enough_available()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2002}));
	assertNotExecuted(book, Order::BuyLimit(InstrumentId{1}, Quantity{200}, Price{2001}).FullOrNothing());
	assertEquals(3, book.Size());
}

void all_or_nothing_limit_order_should_be_executed_if_enough_available()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{150}, Price{2001}));
	assertExecutedAt(book, 2000, 100, 2001, 100, (Order::BuyLimit(InstrumentId{1}, Quantity{200}, Price{2001}).FullOrNothing()));
	assertEquals(1, book.Size());
}

void immediate_or_cancel_all_or_nothing_order_should_be_canceled_if_not_filled()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2002}));
	assertNotExecuted(book, Order::BuyLimit(InstrumentId{1}, Quantity{200}, Price{2001}).FullOrNothing().ImmediateOrCancel());
	assertEquals(2, book.Size());
}

void partial_limit_order_should_be_filled_partially_if_not_enough_available_and_remaining_should_be_canceled()
{
	auto book = BuildOrderBook(1);

	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{100}, Price{2000}));
	assertNotExecuted(book, Order::SellLimit(InstrumentId{1}, Quantity{50}, Price{2002}));
	assertExecutedAt(book, 2000, 100, Order::BuyLimit(InstrumentId{1}, Quantity{200}, Price{2001}).Partial().ImmediateOrCancel());
	assertEquals(1, book.Size());
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

	// partial/full exec of market order
	test("partial_market_order_should_be_filled_partially_if_not_enough_available", partial_market_order_should_be_filled_partially_if_not_enough_available);
	test("all_or_nothing_market_order_should_be_canceled_if_not_enough_available", all_or_nothing_market_order_should_be_canceled_if_not_enough_available);
	test("all_or_nothing_market_order_should_be_executed_if_enough_quantity", all_or_nothing_market_order_should_be_executed_if_enough_quantity);

	// partial/full exec of limit order
	test("partial_limit_order_should_be_filled_partially_if_not_enough_available", partial_limit_order_should_be_filled_partially_if_not_enough_available);
	test("all_or_nothing_limit_order_should_be_inserted_in_book_if_not_enough_available", all_or_nothing_limit_order_should_be_inserted_in_book_if_not_enough_available);
	test("all_or_nothing_limit_order_should_be_executed_if_enough_available", all_or_nothing_limit_order_should_be_executed_if_enough_available);

	// time in force (immediate or cancel)
	test("immediate_or_cancel_all_or_nothing_order_should_be_canceled_if_not_filled", immediate_or_cancel_all_or_nothing_order_should_be_canceled_if_not_filled);
	test("partial_limit_order_should_be_filled_partially_if_not_enough_available_and_remaining_should_be_canceled", partial_limit_order_should_be_filled_partially_if_not_enough_available_and_remaining_should_be_canceled);

    return 0;
}