#ifndef MATRIX_H_
#define MATRIX_H_

#ifdef MATRIX_VERBOSE
#include <iostream>
#endif

#ifdef USEOPENMP
#include <omp.h>
#endif

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

		#ifdef USEOPENMP
		const Matrix<T,R,C> operator+ (const Matrix<T,R,C>& rhs) const
		{
			#ifdef MATRIX_VERBOSE
			int complete = 0;
			#endif
			Matrix<T,R,C> m;
			#pragma omp parallel for
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					m.data[i][j] = data[i][j] + rhs.data[i][j];

					#ifdef MATRIX_VERBOSE
						#pragma omp atomic
						complete++;

						#pragma omp critical
						{
							std::cout << "\r\tAdding entries " << complete << "/" << R * C << " entries computed       \r" << std::flush;
						}
					#endif
				}
			}

			#ifdef MATRIX_VERBOSE
				std::cout << std::endl;
			#endif
			return m;
		}
		#else
		const Matrix<T,R,C> operator+ (const Matrix<T,R,C>& rhs) const
		{
			Matrix<T,R,C> m;
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < C; j++)
				{
					m.data[i][j] = data[i][j] + rhs.data[i][j];
				}
			}
			return m;
		}
		#endif

		friend Matrix<T,R,C> operator+( const T& lhs, const Matrix<T,R,C>& rhs)
		{
			return lhs * Matrix<T,R,C>::One() + rhs;
		}

		friend Matrix<T,R,C> operator+( const Matrix<T,R,C>& lhs, const T& rhs)
		{
			return rhs * Matrix<T,R,C>::One() + lhs;
		}

		#ifdef USEOPENMP
		const Matrix<T,R,R> operator* (const Matrix<T,C,R>& rhs) const
		{
			#ifdef MATRIX_VERBOSE
			int complete = 0;
			#endif
			Matrix<T,R,R> m;

			T summand[R * C * R];

			omp_lock_t sLock[R * R];

			#pragma omp parallel for
			for (int i = 0; i < R * R; i++)
			{
				omp_init_lock(&sLock[i]);
			}

			#ifdef MATRIX_VERBOSE
				std::cout << "\r\tMultiplying. Computing summands\r" << std::flush;
			#endif

			#pragma omp parallel for
			for (int i = 0; i < R * C * R; i++)
			{
				int j = i / (R * C);
				int k = (i % (R * C))/R;
				int l = (i % (R * C))%R;
				summand[i] = data[j][k] * rhs.data[k][l];
				#ifdef MATRIX_VERBOSE
					#pragma omp atomic
					complete++;

					if ((complete % C) == 0)
					{
						#pragma omp critical
						{
							std::cout << "\r\tMultiplying summands. " << complete << "/" << R * C * R << " entries computed       \r" << std::flush;
						}
					}
				#endif
			}

			#ifdef MATRIX_VERBOSE
				complete = 0;
			#endif

			#pragma omp parallel for
			for (int i = 0; i < R * C * R; i++)
			{
				int j = i / (R * C);
				int l = (i % (R * C))/R;
				int k = (i % (R * C))%R;
				
				if (summand[j * (R * C) + k * (R) + l] != T::Zero())
				{
					omp_set_lock(&sLock[j * R + l]);
					m.data[j][l] = m.data[j][l] + summand[j * (R * C) + k * (R) + l];
					omp_unset_lock(&sLock[j * R + l]);
				}

				#ifdef MATRIX_VERBOSE
					#pragma omp atomic
					complete++;

					if ((complete % C) == 0)
					{
						#pragma omp critical
						{
							std::cout << "\r\tAdding summands. " << complete << "/" << R * C * R << " entries computed           \r" << std::flush;
						}
					}
				#endif
			}

			

			#ifdef MATRIX_VERBOSE
				std::cout << std::endl;
			#endif
			return m;
		}
		#else
		const Matrix<T,R,R> operator* (const Matrix<T,C,R>& rhs) const
		{
			#ifdef MATRIX_VERBOSE
			int complete = 0;
			#endif
			Matrix<T,R,R> m;

			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < R; j++)
				{
					m.data[i][j] = T::Zero();
					for (int k = 0; k < C; k++)
					{
						m.data[i][j] = m.data[i][j] + data[i][k] * rhs.data[k][j];
					}
					#ifdef MATRIX_VERBOSE
					std::cout << "Computing entry (" << i << "," << j << ")    \r" << std::flush;
					#endif
				}
			}
			

			#ifdef MATRIX_VERBOSE
				std::cout << std::endl;
			#endif
			return m;
		}
		#endif

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
