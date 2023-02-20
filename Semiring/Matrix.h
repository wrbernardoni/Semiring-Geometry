#ifndef MATRIX_H_
#define MATRIX_H_

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

		const Matrix<T,R,R> operator* (const Matrix<T,C,R>& rhs) const
		{
			Matrix<T,R,R> m;
			for (int i = 0; i < R; i++)
			{
				for (int j = 0; j < R; j++)
				{
					for (int k = 0; k < C; k++)
						m.data[i][j] = m.data[i][j] + data[i][k] * rhs.data[k][j];
				}
			}
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
					os << ";";
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


#endif
