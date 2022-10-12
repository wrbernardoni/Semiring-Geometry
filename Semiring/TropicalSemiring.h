#include <algorithm>
#include <limits>
#include <iostream>

namespace Semiring
{
	class TropicalSemiring
	{
	protected:
		double x;

	public:
		TropicalSemiring()
		{
			x = std::numeric_limits<double>::infinity();
		}

		TropicalSemiring(double n)
		{
			x = n;
		}

		const TropicalSemiring operator+ (const TropicalSemiring& rhs) const
		{

			return TropicalSemiring(std::min(x, rhs.x));
		}

		const TropicalSemiring operator* (const TropicalSemiring& rhs) const
		{

			return TropicalSemiring(x + rhs.x);
		}

		TropicalSemiring& operator= (const TropicalSemiring& rhs)
		{
			x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const TropicalSemiring& ts)
		{
			os << ts.x;
			return os;
		}

		static TropicalSemiring Zero()
		{
			return TropicalSemiring();
		}

		static TropicalSemiring One()
		{
			return TropicalSemiring(0);
		}
	};

}
