#ifndef M_TIMEINFORCE_HPP
#define M_TIMEINFORCE_HPP

namespace M
{
	enum class TimeInForce
	{
		ImmediateOrCancel = 0,
		Day = 1,
		GoodTilCanceled = 1 // nothing more than 'day' for now
	};
}

#endif //M_TIMEINFORCE_HPP
