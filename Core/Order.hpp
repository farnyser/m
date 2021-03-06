#ifndef M_ORDER_HPP
#define M_ORDER_HPP

#include <Core/InstrumentId.hpp>
#include <Core/Price.hpp>
#include <Core/Quantity.hpp>
#include <Core/Type.hpp>
#include <Core/Direction.hpp>
#include "Fulfillment.h"
#include "TImeInForce.hpp"

namespace M
{
    struct Order
    {
        InstrumentId instrument;
        Quantity quantity;
        Price price;
        Direction direction;
        Type type;
		Fulfillment fulfillment;
		TimeInForce timeInForce;
		
		Order(InstrumentId id, Quantity quantity, Price price, Direction direction, Type type, Fulfillment fulfillment = Fulfillment::Partial, TimeInForce tif = TimeInForce::Day)
		: instrument(id), quantity(quantity), price(price), direction(direction), type(type), fulfillment(fulfillment), timeInForce(tif)
		{
		}
		
		static Order BuyLimit(InstrumentId id, Quantity quantity, Price price)
		{
			return Order{id, quantity, price, Direction::Buy, Type::Limit};
		}
		
		static Order SellLimit(InstrumentId id, Quantity quantity, Price price)
		{
			return Order{id, quantity, price, Direction::Sell, Type::Limit};
		}		
		
		static Order Buy(InstrumentId id, Quantity quantity)
		{
			return Order{id, quantity, 0, Direction::Buy, Type::Market};
		}
		
		static Order Sell(InstrumentId id, Quantity quantity)
		{
			return Order{id, quantity, 0, Direction::Sell, Type::Market};
		}

		Order& FullOrNothing()
		{
			fulfillment = Fulfillment::Full;
			return *this;
		}

		Order& Partial()
		{
			fulfillment = Fulfillment::Partial;
			return *this;
		}

		Order& Day()
		{
			timeInForce = TimeInForce::Day;
			return *this;
		}

		Order& ImmediateOrCancel()
		{
			timeInForce = TimeInForce::ImmediateOrCancel;
			return *this;
		}
    };
}

#endif //M_ORDER_HPP
