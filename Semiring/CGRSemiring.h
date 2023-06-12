#ifndef CGR_SEMIRING_H_
#define CGR_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>

// TODO implement this

// This is the minimal semiring to do contact graph routing
// elements are sums + products of contacts (start time, end time, one way light time)
// and storage
// This is over the boolean semiring for now

namespace Semiring
{
	class Contact
	{
	private:
		
	public:
		double start;
		double end;
		double delay;
		bool empty;
		bool lInf;
		bool rInf;

		Contact()
		{
			start = end = delay = 0.0;
			empty = false;
			lInf = true;
			rInf = true;
		}

		Contact(bool em)
		{
			start = end = delay = 0.0;
			empty = em;
			lInf = true;
			rInf = true;
		}

		Contact(double s, double e, double w, bool lI = false, bool rI = false)
		{
			start = s;
			end = e;
			delay = w;
			lInf = lI;
			rInf = rI;
			empty = false;

			if (((e < s) && !(lInf || rInf)) || delay < 0)
				empty = true;
		}

		const Contact operator* (const Contact& rhs) const
		{
			bool lI = lInf && rhs.lInf;
			bool rI = rInf && rhs.rInf;
			bool em = empty || rhs.empty;
			if (em)
				return Contact(em);
			double s;
			double e;
			if (lI)
			{
				s = 0;
			}
			else if (lInf)
			{
				s = rhs.start;
			}
			else if (rhs.lInf)
			{
				s = start;
			}
			else
			{
				s = std::max(start, rhs.start - delay);
			}
			
			if (rI)
			{
				e = 0;
			}
			else if (rInf)
			{
				e = rhs.end - delay;
			}
			else if (rhs.rInf)
			{
				e = end;
			}
			else
			{
				e = std::min(end, rhs.end - delay);
			}

			if ((e < s) && !(lI || rI))
			{
				return Contact(true);
			}

			double d = delay + rhs.delay;

			return Contact(s,e,d,lI,rI);
		}

		Contact& operator= (const Contact& rhs)
		{
			start = rhs.start;
			end = rhs.end;
			delay = rhs.delay;
			lInf = rhs.lInf;
			rInf = rhs.rInf;
			empty = rhs.empty;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const Contact& ts)
		{
			if (!ts.empty)
				os << "([" << (ts.lInf ? "-inf" : std::to_string(ts.start)) << "," << (ts.rInf? "inf":std::to_string(ts.end)) << "]:" << ts.delay << ")";
			else
				os << "()";
			return os;
		}
	};

	class CSC_Contact
	{
	private:
		Contact lContact;
		Contact rContact;
		bool justContact;
	public:
		CSC_Contact()
		{
			justContact = false;
		}

		CSC_Contact(Contact c)
		{
			lContact = c;
			justContact = true;
		}

		CSC_Contact& operator= (const CSC_Contact& rhs)
		{
			lContact = rhs.lContact;
			rContact = rhs.rContact;
			justContact = rhs.justContact;
			return *this;
		}

		CSC_Contact& operator= (const Contact& rhs)
		{
			lContact = rhs;
			justContact = true;
			return *this;
		}

		CSC_Contact(Contact l, Contact r)
		{
			if (l.empty || r.empty)
			{
				lContact = Contact(true);
				justContact = true;
				return;
			}

			double a = std::max(l.start + l.delay + r.delay, r.start + r.delay);
			if (l.lInf)
				a = r.start + r.delay;
			else if (r.lInf)
			{
				a = l.start + l.delay + r.delay;
			}

			bool aMI = l.lInf && r.lInf;
			double b = r.end + r.delay;
			double bI = r.rInf;
			if ((b < a) && !aMI && !bI)
			{
				lContact = Contact(true);
				justContact = true;
				return;
			}
			double c = l.start;
			bool cMI = l.lInf;

			double d = std::max(l.start, r.start - l.delay);
			if (l.lInf)
				d = r.start - l.delay;
			else if (r.lInf)
				d = l.start;

			d = std::min(d,l.end);

			double e = std::min(l.end, r.end - l.delay);
			if (l.rInf)
				e = r.end - l.delay;
			else if (r.rInf)
				e = l.end;

			bool eI = l.rInf && r.rInf;

			if ((d > e) && !eI && !aMI)
			{
				lContact = Contact(true);
				justContact = true;
				return;
			}

			if ((c == e) && (e == d) && (b == a) && !aMI && !bI)
			{
				lContact = Contact(c,e,(aMI ? 0 : a - d));
				justContact = true;
				return;
			}

			lContact = Contact(c,e,0,cMI,eI);
			rContact = Contact(d,d + (b-a), (aMI ? 0 : a - d), aMI, bI);
			// Compute canonical form
			// lContact = l;
			// rContact = r;
			justContact = false;
		}

		const CSC_Contact operator* (const CSC_Contact& rhs) const
		{
			if (justContact && rhs.justContact)
			{
				return CSC_Contact(lContact * rhs.lContact);
			}
			else if (justContact && !rhs.justContact)
			{
				return CSC_Contact(lContact * rhs.lContact, rhs.rContact);
			}
			else if (!justContact && rhs.justContact)
			{
				return CSC_Contact(lContact, rContact * rhs.lContact);
			}
			else
			{
				Contact mid = rContact * rhs.lContact;
				Contact lConj(0, mid.end, 0, true, mid.rInf);
				Contact rConj(mid.start, 0, mid.delay, mid.lInf, true);
				return CSC_Contact(lContact * lConj, rConj * rhs.rContact);
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const CSC_Contact& ts)
		{
			if (!ts.justContact)
				os << ts.lContact << "S" << ts.rContact;
			else
				os << ts.lContact;
			return os;
		}
	};

	class CGRSemiring
	{
	protected:
		bool x;

	public:
		CGRSemiring()
		{
			x = false;
		}

		CGRSemiring(bool n)
		{
			x = n;
		}

		inline friend bool operator==(const CGRSemiring& lhs, const CGRSemiring& rhs) 
		{
			return lhs.x == rhs.x;
		}

		inline friend bool operator!=(const CGRSemiring& lhs, const CGRSemiring& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return (lhs == (lhs + rhs));
		}

		inline friend bool operator>=(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return (rhs == (lhs + rhs));
		}

		inline friend bool operator<(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return (lhs <= rhs) && (lhs != rhs);
		}

		inline friend bool operator>(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return (lhs >= rhs) && (lhs != rhs);
		}

		const CGRSemiring operator+ (const CGRSemiring& rhs) const
		{

			return CGRSemiring(x || rhs.x);
		}

		const CGRSemiring operator* (const CGRSemiring& rhs) const
		{

			return CGRSemiring(x && rhs.x);
		}

		CGRSemiring& operator= (const CGRSemiring& rhs)
		{
			x = rhs.x;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const CGRSemiring& ts)
		{
			os << ts.x;
			return os;
		}

		static CGRSemiring Zero()
		{
			return CGRSemiring();
		}

		static CGRSemiring One()
		{
			return CGRSemiring(true);
		}
	};
}

#endif 
