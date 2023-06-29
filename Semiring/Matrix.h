#ifndef MATRIX_H_
#define MATRIX_H_

#ifdef MATRIX_VERBOSE
#include <iostream>
#endif

#include <omp.h>

namespace Semiring
{
	template<typename T, unsigned int R = 1, unsigned int C = 1> class Matrix
	{
	protected:
		T data[R][C];

	public:
		Matrix<T,R,C>()
		{
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					data[i][j] = T::Zero();
				}
			}
		}

		Matrix<T,R,C>(std::function<T(unsigned int, unsigned int)> n)
		{
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					data[i][j] = n(i,j);
				}
			}
		}

		T& operator()(int a, int b)
		{
			return data[a][b];
		}

		T at(int a, int b) const
		{
			return data[a][b];
		}

		inline friend bool operator==(const Matrix<T,R,C>& lhs, const Matrix<T,R,C>& rhs) 
		{
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					if (lhs.data[i][j] != rhs.data[i][j])
						return false;
				}
			}
			return true;
		}

		inline friend bool operator!=(const Matrix<T,R,C>& lhs, const Matrix<T,R,C>& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const Matrix<T,R,C>& lhs, const Matrix<T,R,C>& rhs)
		{
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const Matrix<T,R,C>& lhs, const Matrix<T,R,C>& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const Matrix<T,R,C>& lhs, const Matrix<T,R,C>& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const Matrix<T,R,C>& lhs, const Matrix<T,R,C>& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		const Matrix<T,R,C> operator+ (const Matrix<T,R,C>& rhs) const
		{
			Matrix<T,R,C> m;
			#pragma omp parallel for
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					m.data[i][j] = data[i][j] + rhs.data[i][j];
				}
			}
			return m;
		}

		friend Matrix<T,R,C> operator+( const T& lhs, const Matrix<T,R,C>& rhs)
		{
			return lhs * Matrix<T,R,C>::One() + rhs;
		}

		friend Matrix<T,R,C> operator+( const Matrix<T,R,C>& lhs, const T& rhs)
		{
			return rhs * Matrix<T,R,C>::One() + lhs;
		}

		// Computes AXA
		friend Matrix<T,R,R> Conjugate(const Matrix<T,R,R> X, const Matrix<T,R,R> A)
		{
			#ifdef MATRIX_VERBOSE
			int complete = 0;
			#endif

			Matrix<T,R,R> m;

			T sum[R * R];
			omp_lock_t sLock[R * R];

			#pragma omp parallel for
			for (int i = 0; i < R * R; i++)
			{
				sum[i] = T::Zero();
				omp_init_lock(&sLock[i]);
			}

			#pragma omp parallel for
			for (int it = 0; it < R * R * R * R; it++)
			{
				int k = it / (R * R * R);
				int l = (it % (R * R * R))/(R * R);
				int i = ((it % (R * R * R))%(R * R))/R;
				int j = ((it % (R * R * R))%(R * R))%R;
				// #pragma omp critical
				// {
				// 	std::cout << it << " " << i << " " << j << " " << k << " " << l << std::endl;
				// }


				T add = A.at(i,l) * X.at(l,k) * A.at(k,j);

				if (add != T::Zero())
				{
					omp_set_lock(&sLock[R * i + j]);
					sum[R * i + j] = sum[R * i + j] + add;	
					omp_unset_lock(&sLock[R * i + j]);
				}
				
				#ifdef MATRIX_VERBOSE
				#pragma omp critical
				{
					complete++;
					if (complete % (R * R) == 0)
						std::cout << "\rConjugating. " << complete / (R * R) << "/" << R * R << " entries computed\r" << std::flush;
				}
				#endif
			}

			#pragma omp parallel for
			for (int i = 0; i < R * R; i++)
			{
				m.data[i / R][i % R] = sum[i];
			}

			#ifdef MATRIX_VERBOSE
					std::cout << std::endl;
			#endif
			return m;
		}

		const Matrix<T,R,R> operator* (const Matrix<T,C,R>& rhs) const
		{
			#ifdef MATRIX_VERBOSE
			int complete = 0;
			#endif
			Matrix<T,R,R> m;
			#pragma omp parallel for
			for (int i = 0; i < R * R; i++)
			{
				for (int k = 0; k < C; k++)
					m.data[i/R][i%R] = m.data[i/R][i%R] + data[i/R][k] * rhs.data[k][i%R];
				#ifdef MATRIX_VERBOSE
				#pragma omp critical
				{
					complete++;
					std::cout << "\r\tMultiplying. " << complete << "/" << R * R << " entries computed\r" << std::flush;
				}
				#endif
			}

			#ifdef MATRIX_VERBOSE
					std::cout << std::endl;
			#endif
			return m;
		}

		const Matrix<T,R,C> operator* (const T& rhs) const
		{
			Matrix<T,R,C> m;
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					m.data[i][j] = data[i][j] * rhs;
				}
			}
			return m;
		}
		friend Matrix<T,R,C> operator*( const T& lhs, const Matrix<T,R,C>& rhs)
		{
			Matrix<T,R,C> m;
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					m.data[i][j] = lhs * rhs.data[i][j];
				}
			}
			return m;

			// return rhs * lhs;
		}

		Matrix<T,R,C>& operator= (const Matrix<T,R,C>& rhs)
		{
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					data[i][j] = rhs.data[i][j];
				}
			}
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const Matrix<T,R,C>& ts)
		{
			os << "{";
			for (int i = 0; i < R; i++)
			{
				os << "{";
				for (int j = 0; j < C; j++)
				{
					os << ts.data[i][j];
					if (j != C - 1)
						os << ", ";
				}
				os << "}";
				if (i != R - 1)
					os << ";\n";
			}

			os << "}";
			return os;
		}

		static Matrix<T,R,C> Zero()
		{
			return Matrix<T,R,C>();
		}

		static Matrix<T,R,C> One()
		{
			Matrix<T,R,C> m;
			for (int i = 0; i < std::min(R,C); i++)
				m.data[i][i] = T::One();
			return m;
		}
	};

}

#include <SetUtilities.h>
#include <functional>

template<typename T, unsigned int R, unsigned int C>
struct std::hash<Semiring::Matrix<T,R,C>>
{
	std::size_t operator()(const Semiring::Matrix<T,R,C>& k) const
	{
		std::size_t minH = 0;
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				minH = minH ^ std::hash<T>()(k.at(i,j));
			}
		}
		return minH;
	}
};


#endif
