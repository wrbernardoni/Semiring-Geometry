#ifndef FREE_MONOID_H_
#define FREE_MONOID_H_

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <SetUtilities.h>

namespace Semiring
{
	// Free monoid on N generators
	template <unsigned int N = 1>
	class FreeMonoid
	{
	protected:
		std::vector<unsigned int> x;
		std::function<std::string(unsigned int)> label;

	public:
		FreeMonoid<N>()
		{
			label = [](unsigned int i)->std::string{ return (i != 0 ? std::to_string(i) : "-");};
		}

		FreeMonoid<N>(std::vector<unsigned int> n)
		{
			label = [](unsigned int i)->std::string{ return (i != 0 ? std::to_string(i) : "-");};
			for (int i = 0; i < n.size(); i++)
				x.push_back(n[i]);
		}

		FreeMonoid<N>(unsigned int n)
		{
			label = [](unsigned int i)->std::string{ return (i != 0 ? std::to_string(i) : "-");};
			if ((n > 0) && ((n <= N)||(N==0)))
				x.push_back(n);
		}

		void setLabel(std::function<std::string(unsigned int)> l)
		{
			label = l;
		}

		inline friend bool operator==(const FreeMonoid<N>& lhs, const FreeMonoid<N>& rhs) 
		{
			if (lhs.x.size() != rhs.x.size())
				return false;
			for (int i = 0; i < lhs.x.size(); i++)
			{
				if (lhs.x[i] != rhs.x[i])
					return false;
			}
			return true;
		}

		inline friend bool operator!=(const FreeMonoid<N>& lhs, const FreeMonoid<N>& rhs) 
		{
			return !(lhs == rhs);
		}

		const FreeMonoid<N> operator* (const FreeMonoid<N>& rhs) const
		{
			std::vector<unsigned int> ret;
			for (int i = 0; i < x.size(); i++)
			{
				ret.push_back(x[i]);
			}
			for (int i = 0; i < rhs.x.size(); i++)
			{
				ret.push_back(rhs.x[i]);
			}

			auto r = FreeMonoid<N>(ret);
			r.setLabel(label);

			return r;
		}

		FreeMonoid<N>& operator= (const FreeMonoid<N>& rhs)
		{
			x = rhs.x;
			label = rhs.label;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const FreeMonoid<N>& ts)
		{
			if (ts.x.size() == 0)
			{
				os << "[";
				os << ts.label(0);
				os << "]";
			}
			else
			{
				os <<"[";
				for (int i = 0; i < ts.x.size(); i++)
				{
					if (i != 0)
						os << " ";
					os << ts.label(ts.x[i]);
				}
				os << "]";
			}

			return os;
		}

		unsigned int& operator()(int a)
		{
			return x[a];
		}

		unsigned int at(int a) const
		{
			return x[a];
		}

		unsigned int size() const
		{
			return x.size();
		}

		void push_back(unsigned int n)
		{
			if ((n > 0) && ((n <= N)||(N==0)))
			{
				x.push_back(n);
			}
		}

		static FreeMonoid<N> One()
		{
			return FreeMonoid<N>();
		}
	};

	
}

#include <SetUtilities.h>
#include <functional>

template<unsigned int N>
struct std::hash<Semiring::FreeMonoid<N>>
{
	std::size_t operator()(const Semiring::FreeMonoid<N>& k) const
	{
		std::size_t minH = 0;
		for (int i = 0; i < k.size(); i++)
		{
			minH = minH ^ std::hash<unsigned int>()(k.at(i));
		}
		return minH;
	}
};

#endif 

