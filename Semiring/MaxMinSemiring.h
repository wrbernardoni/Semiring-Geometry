#ifndef MAX_MIN_SEMIRING_H_
#define MAX_MIN_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>

namespace Semiring
{
	class MaxMinSemiring
	{
	protected:
		double x;

	public:
		inline double get() const {return x; }

		MaxMinSemiring()
		{
			x = 0;
		}

		MaxMinSemiring(double n)
		{
			if (n < 0)
				x = 0;
			else
				x = n;
		}

		inline friend bool operator==(const MaxMinSemiring& lhs, const MaxMinSemiring& rhs) 
		{
			return lhs.x == rhs.x;
		}

		inline friend bool operator!=(const MaxMinSemiring& lhs, const MaxMinSemiring& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const MaxMinSemiring& lhs, const MaxMinSemiring& rhs)
		{
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const MaxMinSemiring& lhs, const MaxMinSemiring& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const MaxMinSemiring& lhs, const MaxMinSemiring& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const MaxMinSemiring& lhs, const MaxMinSemiring& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		const MaxMinSemiring operator+ (const MaxMinSemiring& rhs) const
		{

			return MaxMinSemiring(std::max(x, rhs.x));
		}

		const MaxMinSemiring operator* (const MaxMinSemiring& rhs) const
		{

			return MaxMinSemiring(std::min(x, rhs.x));
		}

		MaxMinSemiring& operator= (const MaxMinSemiring& rhs)
		{
			x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const MaxMinSemiring& ts)
		{
			os << ts.x;
			return os;
		}

		static MaxMinSemiring Zero()
		{
			return MaxMinSemiring(0);
		}

		static MaxMinSemiring One()
		{
			return MaxMinSemiring(std::numeric_limits<float>::infinity());
		}
	};

}

#include <SetUtilities.h>
#include <functional>

template<>
struct std::hash<Semiring::MaxMinSemiring>
{
	std::size_t operator()(const Semiring::MaxMinSemiring& k) const
	{
		return std::hash<double>()(k.get());
	}
};

#endif 
