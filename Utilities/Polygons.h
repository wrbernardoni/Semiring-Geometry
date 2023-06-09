#ifndef POLYGONS_H_
#define POLYGONS_H_

#include <list>
#include <iostream>

// TODO: Figure out how to display/print polygons (especially since hashing is off of print)
// TODO: Do the 3d intersections and shadows ect.

namespace Semiring
{
	namespace Polyhedral
	{
		class PolygonCollection;

		// Has list of points outlining exterior boundary and vector of list of points
		// outlining different interior boundaries
		// 
		// Is one contiguous body
		class Polygon
		{
		private:
			long double boundingBox;
		public:
			Polygon();

			void AdjustBoundingBox(long double nbb);
			Polygon Transpose() const;
			// Applies the matrix [[a,b],[c,d]];
			Polygon ApplyMatrix(double a, double b, double c, double d) const;

			inline long double GetBoundingBox() { return boundingBox; };

			friend bool SubsetEq(const Polygon& lhs, const Polygon& rhs);

			friend bool Overlap(const Polygon& lhs, const Polygon& rhs);

			friend Polygon Union(const Polygon& lhs, const Polygon& rhs);

			friend PolygonCollection Intersect(const Polygon& lhs, const Polygon& rhs);

			friend PolygonCollection SetDifference(const Polygon& lhs, const Polygon& rhs);

			friend PolygonCollection Multiply(const Polygon& lhs, const Polygon& rhs);

			Polygon& operator= (const Polygon& rhs);

			inline friend bool operator<=(const Polygon& lhs, const Polygon& rhs)
			{
				return SubsetEq(lhs, rhs);
			}

			inline friend bool operator==(const Polygon& lhs, const Polygon& rhs) 
			{
				return (lhs <= rhs) && (rhs <= lhs);
			}

			inline friend bool operator!=(const Polygon& lhs, const Polygon& rhs) 
			{
				return !(lhs == rhs);
			}

			inline friend bool operator>=(const Polygon& lhs, const Polygon& rhs)
			{
				return rhs <= lhs;
			}

			inline friend bool operator<(const Polygon& lhs, const Polygon& rhs)
			{
				return (lhs <= rhs) && !(rhs <= lhs);
			}

			inline friend bool operator>(const Polygon& lhs, const Polygon& rhs)
			{
				return (lhs >= rhs) && !(rhs >= lhs);
			}

			friend std::ostream& operator<<(std::ostream& os, const Polygon& ts)
			{
				os << "[";
				// TODO: Print polygon

				os << "]";
				return os;
			}
		};

		// List of disjoint polygons -- each polygon must be fully disjoint from one another
		// i.e. if they overlap at even one point then they must be contained in a single polygon
		class PolygonCollection
		{
		private:

		public:
			std::list<Polygon> polygons;

			PolygonCollection()
			{

			}

			// Add in a polygon to the collection
			PolygonCollection& Add(const Polygon add)
			{
				std::list<Polygon> gons;
				Polygon p = add;

				for (auto poly : polygons)
				{
					if (Overlap(poly, p))
					{
						p = Union(poly, p);
					}
					else
					{
						gons.push_back(poly);
					}
				}

				if (p != Polygon())
				{
					gons.push_back(p);
				}
				
				polygons = gons;

				return *this;
			}

			// Check that a given polygon is contained in the collection
			friend bool SubsetEq(const Polygon& lhs, const PolygonCollection& rhs)
			{
				for (auto p : rhs.polygons)
				{
					if (SubsetEq(lhs, p))
					{
						return true;
					}
				}
				return false;
			}

			// Check that our collection is contained in a given polygon
			friend bool SubsetEq(const PolygonCollection& lhs, const Polygon& rhs)
			{
				for (auto p : lhs.polygons)
				{
					if (!SubsetEq(p, rhs))
					{
						return false;
					}
				}
				return true;
			}

			// Check that our collection is contained in another collection
			friend bool SubsetEq(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				for (auto p : lhs.polygons)
				{
					if (!SubsetEq(p, rhs))
					{
						return false;
					}
				}
				return true;
			}

			friend PolygonCollection Union(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				PolygonCollection uC;

				for (auto p : lhs.polygons)
				{
					uC.Add(p);
				}

				for (auto p : rhs.polygons)
				{
					uC.Add(p);
				}

				return uC;
			}

			friend PolygonCollection Intersect(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				PolygonCollection iN;

				for (auto pL : lhs.polygons)
				{
					for (auto pR : rhs.polygons)
					{
						iN = Union(iN, Intersect(pL, pR));
					}
				}

				return iN;
			}

			friend PolygonCollection SetDifference(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				PolygonCollection sD;
				for (auto pL : lhs.polygons)
				{
					PolygonCollection runningC;
					runningC.Add(pL);

					for (auto pR : rhs.polygons)
					{
						runningC = Intersect(runningC, SetDifference(pL,pR));
					}

					for (auto pI : runningC.polygons)
					{
						sD.Add(pI);
					}
				}

				return sD;
			}

			PolygonCollection ApplyMatrix(double a, double b, double c, double d) const
			{
				PolygonCollection ret;
				for (auto p : polygons)
				{
					ret.Add(p.ApplyMatrix(a,b,c,d));
				}

				return ret;
			}

			PolygonCollection Transpose() const
			{
				PolygonCollection ret;
				for (auto p : polygons)
				{
					ret.Add(p.Transpose());
				}

				return ret;
			}

			// Treat the polygons as RxR matrices and multiply them
			friend PolygonCollection Multiply(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				PolygonCollection runningMult;

				for (auto pL : lhs.polygons)
				{
					for (auto pR : rhs.polygons)
					{
						runningMult = Union(runningMult, Multiply(pL, pR));
					}
				}

				return runningMult;
			}

			PolygonCollection& operator= (const PolygonCollection& rhs)
			{
				polygons.clear();
				for (auto p : rhs.polygons)
				{
					Polygon copy = p;
					polygons.push_back(copy);
				}
				return *this;
			}

			inline friend bool operator<=(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return SubsetEq(lhs, rhs);
			}

			inline friend bool operator==(const PolygonCollection& lhs, const PolygonCollection& rhs) 
			{
				return (lhs <= rhs) && (rhs <= lhs);
			}

			inline friend bool operator!=(const PolygonCollection& lhs, const PolygonCollection& rhs) 
			{
				return !(lhs == rhs);
			}

			inline friend bool operator>=(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return rhs <= lhs;
			}

			inline friend bool operator<(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return (lhs <= rhs) && !(rhs <= lhs);
			}

			inline friend bool operator>(const PolygonCollection& lhs, const PolygonCollection& rhs)
			{
				return (lhs >= rhs) && !(rhs >= lhs);
			}

			friend std::ostream& operator<<(std::ostream& os, const PolygonCollection& ts)
			{
				bool after = false;
				os << "{";
				for (auto p : ts.polygons)
				{
					if (after)
						os << ", ";
					else
						after = true;
					os << p;
				}

				os << "}";
				return os;
			}
		};

	};
};

#endif
