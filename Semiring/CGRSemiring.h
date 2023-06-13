#ifndef CGR_SEMIRING_H_
#define CGR_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>
#include <list>

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

		// If xInf > 0 then x is seen as positive infinity, if xInf < 0 then x is seen as -inf
		bool Contains(double x, double y, int xInf = 0, int yInf = 0) const
		{
			if (justContact)
			{
				if (yInf < 0)
				{
					if (xInf >= 0)
						return false;

					if (!lContact.lInf)
						return false;
				}
				else if (yInf > 0)
				{
					if (xInf <= 0)
						return false;

					if (!lContact.rInf)
						return false;
				}
				else
				{
					if (xInf != 0)
						return false;

					if ((y < lContact.start) || (y > lContact.end))
						return false;

					if (x != (y + lContact.delay))
						return false;
				}

				return true;
			}

			if (xInf > 0)
			{
				if (!rContact.rInf)
				{
					return false;
				}

				if (yInf < 0)
				{
					return lContact.lInf;
				}
				else if (yInf > 0)
				{
					return lContact.rInf;
				}
				else
				{
					if (lContact.lInf && lContact.rInf)
					{
						return true;
					}
					else if (lContact.lInf)
					{
						return (y <= lContact.end);
					}
					else if (lContact.rInf)
					{
						return (y >= lContact.start);
					}
					else
					{
						return (y <= lContact.end) && (y >= lContact.start);
					}
				}
			}
			else if (xInf < 0)
			{
				if (yInf >= 0)
					return false;
				return rContact.lInf && lContact.lInf;
			}
			else
			{
				if (yInf < 0)
				{
					if (!lContact.lInf)
						return false;

					// Check that x is between a and b
					if (!rContact.lInf)
					{
						if (x < (rContact.delay + rContact.start))
							return false;
					}

					if (!rContact.rInf)
					{
						if (x > (rContact.end + rContact.delay))
							return false;
					}

					return true;
				}
				else if (yInf > 0)
				{
					return false;
				}

				// x and y finite

				// Check y is between c and e
				if (!(lContact.lInf))
				{
					if (y < (lContact.start))
						return false;
				}
				
				if (!lContact.rInf)
				{
					if (y > (lContact.end))
						return false;
				}

				// Check if x is contained
				// Check if in sloping region or flat
				if ((rContact.lInf) || (y >= rContact.start))
				{
					// Sloping region
					// check if x is between y + delay and b
					if (x < (y + rContact.delay))
						return false;

					if (!rContact.rInf)
					{
						if (x > (rContact.end + rContact.delay))
							return false;
					}
				}
				else
				{
					// Flat region check if x is between a and b
					if (!rContact.lInf)
					{
						if (x < (rContact.delay + rContact.start))
							return false;
					}

					if (!rContact.rInf)
					{
						if (x > (rContact.end + rContact.delay))
							return false;
					}
				}

				return true;
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

		inline friend bool operator==(const CSC_Contact& lhs, const CSC_Contact& rhs) 
		{
			return (lhs <= rhs)&&(rhs <= lhs);
		}

		inline friend bool operator!=(const CSC_Contact& lhs, const CSC_Contact& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const CSC_Contact& lhs, const CSC_Contact& rhs)
		{
			if (lhs.justContact)
			{
				if (lhs.lContact.lInf)
				{
					if (!rhs.Contains(0, 0, -1, -1))
						return false;
				}
				else
				{
					if (!rhs.Contains(lhs.lContact.start + lhs.lContact.delay, lhs.lContact.start, 0, 0))
						return false;
				}

				if (lhs.lContact.rInf)
				{
					if (!rhs.Contains(0, 0, 1, 1))
						return false;
				}
				else
				{
					if (!rhs.Contains(lhs.lContact.end + lhs.lContact.delay, lhs.lContact.end, 0, 0))
						return false;
				}

				if (lhs.lContact.lInf && lhs.lContact.rInf)
				{
					if (!rhs.Contains(lhs.lContact.delay, 0, 0, 0))
						return false;
				}

				return true;
			}

			// Check if each corner of lhs is contained in rhs
			bool cInfinite = lhs.lContact.lInf;
			double c = lhs.lContact.start;

			bool eInfinite = lhs.lContact.rInf;
			double e = lhs.lContact.end;

			bool aInfinite = lhs.rContact.lInf;
			double a = lhs.rContact.delay + lhs.rContact.start;

			bool bInfinite = lhs.rContact.rInf;
			double b = lhs.rContact.delay + lhs.rContact.end;

			bool dInfinite = lhs.rContact.lInf;
			double d = lhs.rContact.start;

			if (!rhs.Contains(a,c,(aInfinite? -1 : 0), (cInfinite ? -1 : 0)))
			{
				return false;
			}

			if (!rhs.Contains(b,c,(bInfinite? 1 : 0), (cInfinite ? -1 : 0)))
			{
				return false;
			}

			if (!rhs.Contains(a,d,(aInfinite? -1 : 0), (dInfinite ? -1 : 0)))
			{
				return false;
			}

			if (!rhs.Contains(b,e,(bInfinite? 1 : 0), (eInfinite ? 1 : 0)))
			{
				return false;
			}

			if (!rhs.Contains(e + lhs.rContact.delay,e,(eInfinite? 1 : 0), (eInfinite ? 1 : 0)))
			{
				return false;
			}

			return true;
		}

		inline friend bool operator>=(const CSC_Contact& lhs, const CSC_Contact& rhs)
		{
			return rhs <= lhs;
		}

		inline friend bool operator<(const CSC_Contact& lhs, const CSC_Contact& rhs)
		{
			return (lhs <= rhs) && !(rhs <= rhs);
		}

		inline friend bool operator>(const CSC_Contact& lhs, const CSC_Contact& rhs)
		{
			return rhs < lhs;
		}
	};

	class CGRSemiring
	{
	protected:
		std::list<CSC_Contact> contacts;

	public:
		CGRSemiring()
		{
		}

		CGRSemiring(CSC_Contact c)
		{
			Add(c);
		}

		CGRSemiring(Contact c)
		{
			Add(CSC_Contact(c));
		}

		inline friend bool operator==(const CGRSemiring& lhs, const CGRSemiring& rhs) 
		{
			return (lhs <= rhs)&&(rhs <= lhs);
		}

		inline friend bool operator!=(const CGRSemiring& lhs, const CGRSemiring& rhs) 
		{
			return !(lhs == rhs);
		}

		inline friend bool operator<=(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			// TODO
			return false;
		}

		inline friend bool operator>=(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return rhs <= lhs;
		}

		inline friend bool operator<(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return (lhs <= rhs) && !(rhs <= rhs);
		}

		inline friend bool operator>(const CGRSemiring& lhs, const CGRSemiring& rhs)
		{
			return rhs < lhs;
		}

		const CGRSemiring operator+ (const CGRSemiring& rhs) const
		{
			// TODO
			return CGRSemiring();
		}

		const CGRSemiring operator* (const CGRSemiring& rhs) const
		{
			// TODO
			return CGRSemiring();
		}

		CGRSemiring& Add(CSC_Contact c)
		{
			// TODO

			return *this;
		}

		CGRSemiring& operator= (const CGRSemiring& rhs)
		{
			contacts.clear();
			for (auto c : rhs.contacts)
			{
				CSC_Contact nC = c;
				contacts.push_back(nC);
			}

			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const CGRSemiring& ts)
		{
			os << "{";
			bool first = true;
			for (auto c : ts.contacts)
			{
				if (!first)
				{
					os << ", ";
				}
				else
				{
					first = false;
				}

				os << c;
			}
			os << "}";

			return os;
		}

		static CGRSemiring Zero()
		{
			return CGRSemiring();
		}

		static CGRSemiring One()
		{
			return CGRSemiring(Contact());
		}

		static CGRSemiring Storage()
		{
			return CGRSemiring(CSC_Contact());
		}
	};
}

#endif 
