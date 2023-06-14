#ifndef HOP_LIMITED_SEMIRING_H_
#define HOP_LIMITED_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>

namespace Semiring
{
	template<unsigned int L = 1> class HopLimitedSemiring
	{
	protected:
		unsigned int x;

	public:
		inline unsigned int get() const
		{
			return x;
		}

		HopLimitedSemiring<L>()
		{
			x = 0;
		}

		HopLimitedSemiring<L>(unsigned int n)
		{
			if (n < L)
				x = n;
			else
				x = L;
		}

		inline friend bool operator==(const HopLimitedSemiring<L>& lhs, const HopLimitedSemiring<L>& rhs) 
		{
			return lhs.x == rhs.x;
		}

		inline friend bool operator!=(const HopLimitedSemiring<L>& lhs, const HopLimitedSemiring<L>& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const HopLimitedSemiring<L>& lhs, const HopLimitedSemiring<L>& rhs)
		{
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const HopLimitedSemiring<L>& lhs, const HopLimitedSemiring<L>& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const HopLimitedSemiring<L>& lhs, const HopLimitedSemiring<L>& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const HopLimitedSemiring<L>& lhs, const HopLimitedSemiring<L>& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		const HopLimitedSemiring<L> operator+ (const HopLimitedSemiring<L>& rhs) const
		{

			return HopLimitedSemiring<L>(std::min(x, rhs.x));
		}

		const HopLimitedSemiring<L> operator* (const HopLimitedSemiring<L>& rhs) const
		{
			if (x + rhs.x < L)
				return HopLimitedSemiring<L>(x + rhs.x);
			else
				return HopLimitedSemiring<L>(L);
		}

		HopLimitedSemiring<L>& operator= (const HopLimitedSemiring<L>& rhs)
		{
			x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const HopLimitedSemiring<L>& ts)
		{
			if (ts.x < L)
				os << ts.x;
			else
				os << "inf";
			return os;
		}

		static HopLimitedSemiring<L> Zero()
		{
			return HopLimitedSemiring<L>(L);
		}

		static HopLimitedSemiring<L> One()
		{
			return HopLimitedSemiring<L>(0);
		}
	};

}

#include <SetUtilities.h>
#include <functional>

template<unsigned int L>
struct std::hash<Semiring::HopLimitedSemiring<L>>
{
	std::size_t operator()(const Semiring::HopLimitedSemiring<L>& k) const
	{
		return std::hash<unsigned int>()(k.get());
	}
};

#endif 
