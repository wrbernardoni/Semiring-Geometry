#ifndef CGR_SEMIRING_H_
#define CGR_SEMIRING_H_

#include <algorithm>
#include <limits>
#include <iostream>
#include <list>
#include <set>
#include <map>


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

	struct Interval
	{
		double start;
		double end;
		int startInf;
		int endInf;
		bool empty;

		Interval()
		{
			start = 0;
			end = 0;
			empty = true;
			startInf = 0;
			endInf = 0;
		}

		Interval(double s, double e, int sI = 0, int eI = 0)
		{
			start = s;
			end = e;
			startInf = sI;
			endInf = eI;
			empty = ((startInf == endInf) && (startInf == 0) && (end < start));
		}

		friend std::ostream& operator<<(std::ostream& os, const Interval& ts)
		{
			if (!ts.empty)
				os << "[" << (ts.startInf < 0 ? "-inf" : (ts.startInf > 0 ? "inf" : std::to_string(ts.start)))
				 << "," 
				 << (ts.endInf < 0 ? "-inf" : (ts.endInf > 0 ? "inf" : std::to_string(ts.end))) << "]";
			else
				os << "[]";
			return os;
		}

		Interval& operator= (const Interval& rhs)
		{
			start = rhs.start;
			end = rhs.end;
			empty = rhs.empty;
			startInf = rhs.startInf;
			endInf = rhs.endInf;
			return *this;
		}
	};

	class CGRSemiring;

	class CSC_Contact
	{
	private:
		Contact lContact;
		Contact rContact;
		bool justContact;

		Interval Width(double y, int yInf = 0)
		{
			bool cInfinite = lContact.lInf;
			double c = lContact.start;

			bool eInfinite = lContact.rInf;
			double e = lContact.end;

			bool aInfinite = rContact.lInf;
			double a = rContact.delay + rContact.start;

			bool bInfinite = rContact.rInf;
			double b = rContact.delay + rContact.end;

			bool dInfinite = rContact.lInf;
			double d = rContact.start;

			if (yInf < 0)
			{
				if (lContact.lInf)
				{
					// Get interval [a,b]
					if (aInfinite && bInfinite)
					{
						return Interval(0,0,-1,1);
					}
					else if (aInfinite)
					{
						return Interval(0,b,-1,0);
					}
					else if (bInfinite)
					{
						return Interval(a,0,0,1);
					}
					else
					{
						return Interval(a,b);
					}

				}
				else
				{
					return Interval();
				}
			}
			else if (yInf > 0)
			{
				if (lContact.rInf)
				{
					return Interval(0,0,1,1);
				}
				else
				{
					return Interval();
				}
			}
			else
			{
				// y finite
				if (!eInfinite && (y > e))
					return Interval();

				if (!cInfinite && (y < c))
					return Interval();

				if (!dInfinite && (y < d))
				{
					// In boxy area
					if (bInfinite)
					{
						return Interval(a,0,0,1);
					}
					else
					{
						return Interval(a,b);
					}
				}
				else
				{
					// In slanted area
					if (bInfinite)
					{
						return Interval(y + rContact.delay,0,0,1);
					}
					else
					{
						return Interval(y + rContact.delay,b);
					}
				}
			}
		}

		std::list<double> clippedIntersections(CSC_Contact& other) const
		{
			std::list<double> clips;
			/// Return list of y values where my diagonal clips the other
			bool cInfinite = other.lContact.lInf;
			double c = other.lContact.start;

			bool eInfinite = other.lContact.rInf;
			double e = other.lContact.end;

			bool aInfinite = other.rContact.lInf;
			double a = other.rContact.delay + other.rContact.start;

			bool bInfinite = other.rContact.rInf;
			double b = other.rContact.delay + other.rContact.end;

			bool dInfinite = other.rContact.lInf;
			double d = other.rContact.start;

			if (other.justContact)
				return std::list<double>();

			double delay = 0;

			if (justContact)
			{
				delay = lContact.delay;
				if (other.rContact.delay == lContact.delay)
					return std::list<double>();
			}
			else
			{
				delay = rContact.delay;
				if (other.rContact.delay == rContact.delay)
					return std::list<double>();
			}

			// x = a line
			if (!aInfinite)
			{
				// check if a-delay is in our interval
				if (Contains(a,a-delay) && other.Contains(a,a-delay))
				{
					clips.push_back(a-delay);
				}
			}

			// x = b line
			if (!bInfinite)
			{
				if (Contains(b,b-delay) && other.Contains(b,b-delay))
					clips.push_back(b-delay);
			}

			// y = c line
			if (!cInfinite)
			{
				if (Contains(c+delay,c) && other.Contains(c+delay,c))
					clips.push_back(c);
			}

			// y = e line
			if (!eInfinite)
			{
				if (Contains(e+delay,e) && other.Contains(e+delay,e))
					clips.push_back(e);
			}

			return clips;
		}
	public:

		friend bool operator<=(const CGRSemiring& lhs, const CGRSemiring& rhs);

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

		inline bool isEmpty()
		{
			return lContact.empty;
		}

		CSC_Contact(Contact l, Contact r)
		{
			// std::cout << "Computing canonical form of " << l << "S" << r << std::endl;
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

			if (!l.rInf)
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

			// std::cout << "a" << a << " b" << b << " c" << c << " d" << d << " e" << e << std::endl;

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
			if (lContact.empty)
				return false;

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

		// This is the subset relationship
		inline friend bool operator<=(const CSC_Contact& lhs, const CSC_Contact& rhs)
		{
			if (lhs.lContact.empty)
				return true;

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
		// -1 if no contacts
		double MinDelay() const
		{
			double mD = -1.0;
			bool first = true;


			for (auto c : contacts)
			{
				if (c.isEmpty())
					continue;
				if ((first) || (c.rContact.delay < mD))
				{
					first = false;
					mD = c.rContact.delay;
				}
			}

			return mD;
		}

		// Returns minimum available time / -1272141235.45 if negative infinity
		// Used for hashing only
		double h1() const
		{
			double mD = -1.0;
			bool first = true;


			for (auto c : contacts)
			{
				if (c.isEmpty())
					continue;

				bool aInfinite = c.rContact.lInf;
				double a = c.rContact.delay + c.rContact.start;
				if ((first) || (a < mD) || (aInfinite))
				{
					if (aInfinite)
						return -1272141235.45;

					first = false;
					mD = a;
				}
			}

			return mD;
		}

		// Returns minimum available time / 7123891247891212.45 if infinity
		// Used for hashing only
		double h2() const
		{
			double mD = -1.0;
			bool first = true;


			for (auto c : contacts)
			{
				if (c.isEmpty())
					continue;

				bool bInfinite = c.rContact.rInf;
				double b = c.rContact.delay + c.rContact.end;
				if ((first) || (b > mD) || (bInfinite))
				{
					if (bInfinite)
						return 7123891247891212.45;
					
					first = false;
					mD = b;
				}
			}

			return mD;
		}

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
			// Step 1: Make sorted list of special y values (y values of corners of shapes in lhs + rhs)
			std::set<double> corners;
			bool mI = false;
			bool pI = false;

			for (auto c : lhs.contacts)
			{
				if (c.lContact.lInf)
					mI = true;
				else
					corners.insert(c.lContact.start);

				if (c.lContact.rInf)
					pI = true;
				else
					corners.insert(c.lContact.end);

				if (c.rContact.lInf)
				{
					mI = true;
				}
				else
					corners.insert(c.rContact.start);
			}

			for (auto c : rhs.contacts)
			{
				if (c.lContact.lInf)
					mI = true;
				else
					corners.insert(c.lContact.start);

				if (c.lContact.rInf)
					pI = true;
				else
					corners.insert(c.lContact.end);

				if (c.rContact.lInf)
				{
					mI = true;
				}
				else
					corners.insert(c.rContact.start);

				for (auto c2 : lhs.contacts)
				{
					auto clips = c2.clippedIntersections(c);
					for (auto clip : clips)
					{
						corners.insert(clip);
					}
				}
			}

			// std::cout << "Corners" << std::endl;
			// for (auto c : corners)
			// {
			// 	std::cout << c << std::endl;
			// }
			// std::cout << "n" << std::endl;

			// Step 2: Make list of midpoints of the above values
			std::set<double> midPoints;

			if (mI && (corners.size() != 0))
			{
				midPoints.insert((*corners.begin()) - 1.0);
			}
			else if (mI && pI)
			{
				midPoints.insert(0);
			}
			
			if (corners.size() != 0)
			{
				double pVal = (*corners.begin());
				auto it = corners.begin();
				it++;
				while (it != corners.end())
				{
					midPoints.insert(((*it) + pVal)/2.0);
					pVal = *it;
					it++;
				}

				if (pI)
				{
					midPoints.insert(pVal + 1.0);
				}
			}

			// std::cout << "MP" << std::endl;
			for (auto m : midPoints)
			{
				int leftMIBracket = 0;
				int leftPIBracket = 0;
				std::map<double, int> leftbrackets;
				// Step 3: For each midpoint draw a line and segment the line at the edges of shapes in lhs + rhs
				for (auto c : lhs.contacts)
				{
					Interval w = c.Width(m);
					if (w.empty)
						continue;
					// std::cout << w << std::endl;
					if (w.startInf < 0)
					{
						leftMIBracket++;
					}
					else if (w.startInf > 0)
					{
						leftPIBracket++;
					}
					else
					{
						if (leftbrackets.count(w.start) != 0)
							leftbrackets[w.start]++;
						else
							leftbrackets[w.start] = 1;
					}

					if (w.endInf < 0)
					{
						leftMIBracket--;
					}
					else if (w.endInf > 0)
					{
						leftPIBracket--;
					}
					else
					{
						if (leftbrackets.count(w.end) != 0)
							leftbrackets[w.end]--;
						else
							leftbrackets[w.end] = -1;
					}
				}

				int rightMIBracket = 0;
				int rightPIBracket = 0;
				std::map<double, int> rightbrackets;
				for (auto c : rhs.contacts)
				{
					Interval w = c.Width(m);
					if (w.empty)
						continue;
					// std::cout << w << std::endl;
					if (w.startInf < 0)
					{
						rightMIBracket++;
					}
					else if (w.startInf > 0)
					{
						rightPIBracket++;
					}
					else
					{
						if (rightbrackets.count(w.start) != 0)
							rightbrackets[w.start]++;
						else
							rightbrackets[w.start] = 1;
					}

					if (w.endInf < 0)
					{
						rightMIBracket--;
					}
					else if (w.endInf > 0)
					{
						rightPIBracket--;
					}
					else
					{
						if (rightbrackets.count(w.end) != 0)
							rightbrackets[w.end]--;
						else
							rightbrackets[w.end] = -1;
					}
				}

				// Step 4: For each segment determine if it is in rhs and not in lhs, if so return false
				int lDepth = leftMIBracket;
				int rDepth = rightMIBracket;
				if ((rDepth > 0) && (lDepth <= 0))
				{
					return false;
				}

				auto lIt = leftbrackets.begin();
				auto rIt = rightbrackets.begin();
				while ((lIt != leftbrackets.end()) || (rIt != rightbrackets.end()))
				{
					if ((lIt != leftbrackets.end()) && (rIt != rightbrackets.end()))
					{
						if (lIt->first < rIt->first)
						{
							lDepth += lIt->second;
							// std::cout << lIt->first << " " << lDepth << " " << rDepth << std::endl;
							lIt++;
						}
						else if (rIt->first < lIt->first)
						{
							rDepth += rIt->second;
							// std::cout << rIt->first << " " << lDepth << " " << rDepth << std::endl;
							rIt ++;
						}
						else
						{
							lDepth += lIt->second;
							rDepth += rIt->second;
							
							// std::cout << lIt->first << " " << lDepth << " " << rDepth << std::endl;
							lIt++;
							rIt ++;
						}
					}
					else if (lIt != leftbrackets.end())
					{
						lDepth += lIt->second;
						// std::cout << lIt->first << " " << lDepth << " " << rDepth << std::endl;
						lIt++;
					}
					else
					{
						rDepth += rIt->second;
						// std::cout << rIt->first << " " << lDepth << " " << rDepth << std::endl;
						rIt ++;
					}

					if ((rDepth > 0) && (lDepth <= 0))
					{
						return false;
					}
				}

				lDepth += leftPIBracket;
				rDepth += rightPIBracket;
				if ((rDepth > 0) && (lDepth <= 0))
				{
					return false;
				}
			}
			// std::cout << "non" << std::endl;

			

			// Step 5: Else return true
			return true;
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
			CGRSemiring s;
			for (auto c : contacts)
			{
				s.Add(c);
			}

			for (auto c : rhs.contacts)
			{
				s.Add(c);
			}

			return s;
		}

		const CGRSemiring operator* (const CGRSemiring& rhs) const
		{
			CGRSemiring s;
			for (auto x : contacts)
			{
				for (auto y : rhs.contacts)
				{
					auto m = x * y;
					if (m != CSC_Contact(Contact(false)));
						s.Add(x * y);
				}
			}

			return s;
		}

		CGRSemiring& Add(CSC_Contact c)
		{
			CSC_Contact dom = c;
			std::list<CSC_Contact> newContacts;
			auto itr = contacts.begin();

			while (itr != contacts.end())
			{
				if ((*itr) >= dom)
				{
					dom = (*itr);
					return *this;
				}
				else if (!(dom >= (*itr)))
				{
					newContacts.push_back(*itr);
				}

				itr++;
			}

			newContacts.push_back(dom);
			contacts = newContacts;

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

#include <SetUtilities.h>

// TODO actually make the hash
template<>
struct std::hash<Semiring::CGRSemiring>
{
	std::size_t operator()(const Semiring::CGRSemiring& k) const
	{


		return std::hash<double>()(k.MinDelay()) ^ std::hash<double>()(k.h1()) ^ std::hash<double>()(k.h2());
	}
};

#endif 
