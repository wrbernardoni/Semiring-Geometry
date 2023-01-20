#ifndef CONTACT_SEMIRING_H_
#define CONTACT_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>
#include <functional>
#include <vector>

// #include <iostream>

namespace Semiring
{
	template<typename T, unsigned int L = 1>  class ContactSemiring
	{
	protected:
		std::vector<std::vector<T>> x;

	public:
		ContactSemiring<T,L>()
		{
			// std::cout << "Here with " << L << std::endl;
			for (int i = 0; i <= L; i++)
			{
				std::vector<T> row;
				for (int j = 0; j <= (L-i); j++)
				{
					row.push_back(T::Zero());
				}

				x.push_back(row);
			}
		}

		ContactSemiring<T,L>(std::function<T(unsigned int, unsigned int)> n)
		{
			for (int i = 0; i <= L; i++)
			{
				std::vector<T> row;
				for (int j = 0; j <= (L-i); j++)
				{
					row.push_back(n(i,j));
				}

				x.push_back(row);
			}
		}

		T& operator()(int t, int d)
		{
			return x[t][d];
		}

		std::vector<T>& operator()(int t)
		{
			return x[t];
		}

		inline friend bool operator==(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs) 
		{
			for (int i = 0; i <= L; i++)
			{
				for (int j = 0; j <= (L-i); j++)
				{
					if (lhs.x[i][j] != rhs.x[i][j])
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
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const ContactSemiring<T,L>& lhs, const ContactSemiring<T,L>& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		const ContactSemiring<T,L> operator+ (const ContactSemiring<T,L>& rhs) const
		{
			ContactSemiring<T,L> s = ContactSemiring<T,L>();
			for (int i = 0; i <= L; i++)
			{
				for (int j = 0; j <= (L-i); j++)
				{
					s(i,j) = x[i][j] + rhs.x[i][j];
				}
			}
			
			return s;
		}

		const ContactSemiring<T,L> operator* (const ContactSemiring<T,L>& rhs) const
		{
			ContactSemiring<T,L> s = ContactSemiring<T,L>();

			for (int t = 0; t <= L; t++)
			{
				for (int d = 0; d <= (L-t); d++)
				{
					for (int z = 0; z <= d; z++)
					{
						s(t,d) = s(t,d) +  rhs.x[t+z][d - z] * x[t][z];
					}
				}
			}

			return s;
		}

		ContactSemiring<T,L>& operator= (const ContactSemiring<T,L>& rhs)
		{
			for (int i = 0; i <= L; i++)
			{
				for (int j = 0; j <= (L-i); j++)
				{
					x[i][j] = rhs.x[i][j];
				}
			}
			// x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const ContactSemiring<T,L>& ts)
		{
			for (int i = 0; i <= L; i++)
			{
				os << "[" << i << "] : ";
				for (int j = 0; j <= (L-i); j++)
				{
					os << "(" << j << ") " << ts.x[i][j] << " ";
				}
				if (i != L)
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
