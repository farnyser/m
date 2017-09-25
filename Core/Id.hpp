#ifndef M_ID_HPP
#define M_ID_HPP

namespace M
{
	template <typename TUnderlying>
	struct Id : TUnderlying
	{
		size_t identifier;

		Id(size_t identifier, const TUnderlying& underlying) : TUnderlying(underlying), identifier(identifier)
		{
		}

		Id(const Id<TUnderlying>& copy) : TUnderlying(copy), identifier(copy.identifier)
		{
		}
	};
}

#endif //M_ID_HPP
