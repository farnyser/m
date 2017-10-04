#ifndef M_HELPER_HPP
#define M_HELPER_HPP

#include <Core/Quantity.hpp>
#include <Core/Type.hpp>

namespace M
{
	template<typename TContainer>
	bool CheckExecutability(const TContainer &orders, Quantity quantity, unsigned int signed_price, Type type)
	{
		for (auto &x : orders)
		{
			if (x.signed_price > signed_price && type != Type::Market)
				return false;

			if (quantity > x.quantity)
				quantity -= x.quantity;
			else
				return true;
		}

		return false;
	}
}

#endif //M_HELPER_HPP
