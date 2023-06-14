#ifndef BOOLEAN_SEMIRING_H_
#define BOOLEAN_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>

namespace Semiring
{
	class BooleanSemiring
	{
	protected:
		bool x;

	public:
		inline bool get() const {return x;}

		BooleanSemiring()
		{
			x = false;
		}

		BooleanSemiring(bool n)
		{
			x = n;
		}

		inline friend bool operator==(const BooleanSemiring& lhs, const BooleanSemiring& rhs) 
		{
			return lhs.x == rhs.x;
		}

		inline friend bool operator!=(const BooleanSemiring& lhs, const BooleanSemiring& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const BooleanSemiring& lhs, const BooleanSemiring& rhs)
		{
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const BooleanSemiring& lhs, const BooleanSemiring& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const BooleanSemiring& lhs, const BooleanSemiring& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const BooleanSemiring& lhs, const BooleanSemiring& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		const BooleanSemiring operator+ (const BooleanSemiring& rhs) const
		{

			return BooleanSemiring(x || rhs.x);
		}

		const BooleanSemiring operator* (const BooleanSemiring& rhs) const
		{

			return BooleanSemiring(x && rhs.x);
		}

		BooleanSemiring& operator= (const BooleanSemiring& rhs)
		{
			x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const BooleanSemiring& ts)
		{
			os << ts.x;
			return os;
		}

		static BooleanSemiring Zero()
		{
			return BooleanSemiring();
		}

		static BooleanSemiring One()
		{
			return BooleanSemiring(true);
		}
	};

}

#include <SetUtilities.h>
#include <functional>

template<>
struct std::hash<Semiring::BooleanSemiring>
{
	std::size_t operator()(const Semiring::BooleanSemiring& k) const
	{
		return k.get();
	}
};

#endif 
