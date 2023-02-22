#ifndef CONTACT_SEMIRING_H_
#define CONTACT_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>
#include <functional>
#include <vector>
#include <map>

// #include <iostream>

namespace Semiring
{
	template<typename T, unsigned int L = 1>  class ContactSemiring
	{
	protected:
		std::map<unsigned int, std::map<unsigned int, T>> conv;

	public:
		ContactSemiring<T,L>()
		{
		}

		ContactSemiring<T,L>(std::function<T(unsigned int, unsigned int)> n)
		{
			for (int i = 0; i <= L; i++)
			{
				for (int j = 0; j <= (L-i); j++)
				{
					T nu = n(i,j);
					
					if (nu != T::Zero())
					{
						conv[i][j] = nu;
					}
				}
			}
		}

		T& operator()(unsigned int t, unsigned int d)
		{
			if (conv.count(t) != 0)
			{
				if (conv[t].count(d) != 0)
				{
					return conv[t][d];
				}
			}
			
			conv[t][d] = T::Zero();
			return conv[t][d];
		}

		T at(unsigned int t, unsigned int d) const
		{
			if (conv.count(t) != 0)
			{
				if (conv.at(t).count(d) != 0)
				{
					T ret = conv.at(t).at(d);
					return ret;
				}
			}

			return T::Zero();
		}

		std::map<unsigned int, T>& operator()(int t)
		{
			return conv[t];
		}

		// TODO: Speed this up
		inline friend bool operator==(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs) 
		{
			for (int i = 0; i <= L; i++)
			{
				for (int j = 0; j <= (L-i); j++)
				{
					if (lhs.at(i,j) != rhs.at(i,j))
						return false;
				}
			}
			return true;
		}

		inline friend bool operator!=(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			for (auto it = lhs.conv.begin(); it != lhs.conv.end(); it++)
			{
				if (rhs.conv.count(it->first) == 0)
					return false;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); it++)
				{
					if (rhs.at(it->first, it2->first) > it2->second)
					{
						return false;
					}
				}
			}
			return true;
		}

		inline friend bool operator>=(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			for (auto it = rhs.conv.begin(); it != rhs.conv.end(); it++)
			{
				if (lhs.conv.count(it->first) == 0)
					return false;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); it++)
				{
					if (lhs.at(it->first, it2->first) < it2->second)
					{
						return false;
					}
				}
			}
			return true;
		}

		inline friend bool operator<(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			for (auto it = lhs.conv.begin(); it != lhs.conv.end(); it++)
			{
				if (rhs.conv.count(it->first) == 0)
					return false;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); it++)
				{
					if (rhs.at(it->first, it2->first) >= it2->second)
					{
						return false;
					}
				}
			}
			return true;
		}

		inline friend bool operator>(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			for (auto it = rhs.conv.begin(); it != rhs.conv.end(); it++)
			{
				if (lhs.conv.count(it->first) == 0)
					return false;
				for (auto it2 = it->second.begin(); it2 != it->second.end(); it++)
				{
					if (lhs.at(it->first, it2->first) <= it2->second)
					{
						return false;
					}
				}
			}
			return true;
		}

		const ContactSemiring<T,L> operator+ (const ContactSemiring<T,L>& rhs) const
		{
			ContactSemiring<T,L> s = ContactSemiring<T,L>();
			for (int i = 0; i <= L; i++)
			{
				for (int j = 0; j <= (L-i); j++)
				{
					T sum = at(i,j) + rhs.at(i,j);
					if (sum != T::Zero())
						s(i,j) = sum;
				}
			}
			
			return s;
		}

		const ContactSemiring<T,L> operator* (const ContactSemiring<T,L>& rhs) const
		{
			ContactSemiring<T,L> s = ContactSemiring<T,L>();

			for (auto itT = conv.begin(); itT != conv.end(); itT++)
			{
				unsigned int t = itT->first;
				for (auto itZ = itT->second.begin(); itZ != itT->second.end(); itZ++)
				{
					unsigned int z = itZ->first;
					if (itZ->second == T::Zero())
						continue;
					if (rhs.conv.count(t + z) == 0)
						continue;
					for (auto itD = rhs.conv.at(t + z).begin(); itD != rhs.conv.at(t + z).end(); itD++)
					{
						unsigned int d = itD->first;
						if (((d + z) > (L - t)))
							continue;
						if (itD->second == T::Zero())
							continue;
						T prod = itD->second * itZ->second;
						s(t,d + z) = s(t,d + z) + prod;
					}
					// for (int d = z; d <= (L - t); d++)
					// {
					// 	if (rhs.at(t + z, d - z) != T::Zero())
					// 	{
					// 		T prod = rhs.at(t + z, d- z) * itZ->second;
					// 		s(t,d) = s(t,d) + prod;
					// 	}
					// }
				}
			}

			return s;
		}

		ContactSemiring<T,L>& operator= (const ContactSemiring<T,L>& rhs)
		{
			for (int i = 0; i <= L; i++)
			{
				conv[i].clear();
				for (int j = 0; j <= (L-i); j++)
				{
					if (rhs.at(i,j) != T::Zero())
						conv[i][j] = rhs.at(i,j);
				}
			}
			// x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const ContactSemiring<T,L>& ts)
		{
			for (int i = 0; i <= L; i++)
			{
				bool added = false;
				for (int j = 0; j <= (L-i); j++)
				{
					if (ts.at(i,j) != T::Zero())
					{
						if (!added)
						{
							added = true;
							os << "[" << i << "] : ";
						}
						os << "(" << j << ") " << ts.at(i,j) << " ";
					}
				}
				if (i != L && added)
					os << "\n";
			}
			// os << ts.x;
			return os;
		}

		static ContactSemiring<T,L> Zero()
		{
			return ContactSemiring<T,L>();
		}

		static ContactSemiring<T,L> One()
		{
			return ContactSemiring<T,L>([](unsigned int t, unsigned int d)->T{
				if (d == 0)
					return T::One();
				else
					return T::Zero();
			});
		}
	};

}

#endif 
