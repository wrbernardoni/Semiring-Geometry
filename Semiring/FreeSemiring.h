#ifndef FREE_IDEMPOTENT_SEMIRING_H_
#define FREE_IDEMPOTENT_SEMIRING_H_

#include <utility>
#include <unordered_set>
#include <string>
#include <sstream>

#include <SetUtilities.h>

namespace Semiring
{
	// Constructs the free idempotent semiring on a monoid defined by T
	// Just need T to be a unital monoid, so it needs T.One(), T.operator==, T.operator<<, and T.operator* defined
	template<typename T> class FreeIdempotentSemiring
	{
	protected:
		std::unordered_set<T> x;

	public:
		FreeIdempotentSemiring<T>()
		{
		}

		FreeIdempotentSemiring<T>(std::unordered_set<T> n)
		{
			x = n;
		}

		FreeIdempotentSemiring<T>(T n)
		{
			x.insert(n);
		}

		inline friend bool operator==(const FreeIdempotentSemiring<T>& lhs, const FreeIdempotentSemiring<T>& rhs) 
		{
			return lhs.x == rhs.x;
		}

		inline friend bool operator!=(const FreeIdempotentSemiring<T>& lhs, const FreeIdempotentSemiring<T>& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const FreeIdempotentSemiring<T>& lhs, const FreeIdempotentSemiring<T>& rhs)
		{
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const FreeIdempotentSemiring<T>& lhs, const FreeIdempotentSemiring<T>& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const FreeIdempotentSemiring<T>& lhs, const FreeIdempotentSemiring<T>& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const FreeIdempotentSemiring<T>& lhs, const FreeIdempotentSemiring<T>& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		bool operator()(T& e)
		{
			return (x.count(e) > 0);
		}

		void insert(T e)
		{
			x.insert(e);
		}

		void remove(T e)
		{
			x.erase(e);
		}

		const FreeIdempotentSemiring<T> operator+ (const FreeIdempotentSemiring<T>& rhs) const
		{
			std::unordered_set<T> result = x;
  			result.insert(rhs.x.begin(), rhs.x.end());
			return FreeIdempotentSemiring<T>(result);
		}

		const FreeIdempotentSemiring<T> operator* (const FreeIdempotentSemiring<T>& rhs) const
		{
			std::unordered_set<T> result;
			for (auto itr = x.begin(); itr != x.end(); itr++)
			{
				for (auto itr2 = rhs.x.begin(); itr2 != rhs.x.end(); itr2++)
				{
					result.insert((*itr) * (*itr2));
				}
			}
			return FreeIdempotentSemiring<T>(result);
		}

		FreeIdempotentSemiring<T>& operator= (const FreeIdempotentSemiring<T>& rhs)
		{
			x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const FreeIdempotentSemiring<T>& ts)
		{
			os << "{";
			bool begin = true;
			for (auto itr = ts.x.begin(); itr != ts.x.end(); itr++)
			{
				if (!begin)
				{
					os <<", ";
				}

				os << (*itr);
				begin = false;
			}
			os << "}";
			return os;
		}

		static FreeIdempotentSemiring<T> Zero()
		{
			return FreeIdempotentSemiring<T>();
		}

		static FreeIdempotentSemiring<T> One()
		{
			std::unordered_set<T> result;
			result.insert(T::One());
			return FreeIdempotentSemiring<T>(result);
		}

		std::unordered_set<T> getSet() const
		{
			return x;
		}
	};
}

#include <SetUtilities.h>
#include <functional>

template<typename T>
struct std::hash<Semiring::FreeIdempotentSemiring<T>>
{
	std::size_t operator()(const Semiring::FreeIdempotentSemiring<T>& k) const
	{
		std::size_t minH = 0;
		bool first = true;
		for (auto x : k.getSet())
		{
			if (((std::hash<T>()(x) + 1) <= minH) || first)
			{
				first = false;
				minH = std::hash<T>()(x) + 1;
			}
		}

		return minH;
	}
};

#endif 
