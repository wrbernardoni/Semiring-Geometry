#include <Polygons.h>
#include <cmath>

using namespace std;
using namespace Semiring;
using namespace Semiring::Polyhedral;


namespace Semiring::Polyhedral
{
	/*	*********************************
			Polygon utility functions
		*********************************/
	bool SubsetEq(const Polygon& lhs, const Polygon& rhs)
	{
		// TODO Check each member of left is a subset of right
		return false;
	}

	bool Overlap(const Polygon& lhs, const Polygon& rhs)
	{
		// TODO check if left and right overlap
		return false;
	}

	Polygon Union(const Polygon& lhs, const Polygon& rhs)
	{
		// TODO **If there is overlap** then union the two polygons, if not return
		// an empty polygon
		return Polygon();
	}

	PolygonCollection Intersect(const Polygon& lhs, const Polygon& rhs)
	{
		// TODO return intersection of two polygons
		return PolygonCollection();
	}

	PolygonCollection SetDifference(const Polygon& lhs, const Polygon& rhs)
	{
		// TODO return left-right
		return PolygonCollection();
	}

	PolygonCollection Multiply(const Polygon& lhs, const Polygon& rhs)
	{
		// TODO return the matrix multiplication of our polygons

		// Step 1: Find rhs transpose
		// Step 2: Explode lhs vertically and rhs as a skew
		// Step 3: Compute the intersection of their 3d shapes
		// Step 4: Find the shadow of the intersection on the e1-e3 plane
		// Step 5: Skew the resulting polygons by [[1,0],[1,-1]] -- this is your resulting polygon

		return PolygonCollection();
	}


	/*	*********************************
			Polygon core functions
		*********************************/
	Polygon::Polygon()
	{
	}

	Polygon::Polygon(Ring bound)
	{
		boundary = bound;
	}

	Polygon& Polygon::operator= (const Polygon& rhs)
	{
		boundary = rhs.boundary;

		interiorRings.clear();
		for (auto iR : rhs.interiorRings)
		{
			Ring nR = iR;
			interiorRings.push_back(iR);
		}
		return *this;

	}

	Polygon Polygon::Transpose() const
	{
		return ApplyMatrix(0,1,1,0);
	}

	Polygon Polygon::ApplyMatrix(double a, double b, double c, double d) const
	{
		// TODO apply the matrix to this polygon and return the result
		// May need to reverse all rings if the determinant is negative
		// If the determinant is zero then maybe kill off all interior rings
		return Polygon();
	}

	/*	*********************************
			Default polygon shapes
		*********************************/
	Polygon Polygon::UnitBox()
	{
		Ring ub;
		ub.vertices.push_back(Point(0,0));
		ub.vertices.push_back(Point(1,0));
		ub.vertices.push_back(Point(1,1));
		ub.vertices.push_back(Point(0,1));

		return Polygon(ub);
	}

	Polygon Polygon::HalfPlane(Point a, Point b)
	{
		Polygon hp;

		double run = b.x - a.x;
		double rise = b.y - a.y;
		if ((run == 0) && (rise == 0))
		{
			// Invalid input
			throw("Ill defined HalfPlane");
		}

		hp.boundary.vertices.push_back(a);
		hp.boundary.vertices.push_back(b);
		hp.boundary.ray = true;
		

		return hp;
	}

	Polygon Polygon::Line(Point a, Point b)
	{
		Polygon hp;

		double run = b.x - a.x;
		double rise = b.y - a.y;
		if ((run == 0) && (rise == 0))
		{
			// Invalid input
			throw("Ill defined Line");
		}

		hp.boundary.vertices.push_back(a);
		hp.boundary.vertices.push_back(b);
		hp.boundary.ray = false;
		

		return hp;
	}

	Polygon Polygon::Dot(Point a)
	{
		Polygon hp;

		hp.boundary.vertices.push_back(a);
		hp.boundary.ray = false;
		

		return hp;
	}


	/*	*********************************
			Polygon Collection utility functions
		*********************************/
	bool SubsetEq(const Polygon& lhs, const PolygonCollection& rhs)
	{
		if (lhs == Polygon())
			return true;

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
	bool SubsetEq(const PolygonCollection& lhs, const Polygon& rhs)
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
	bool SubsetEq(const PolygonCollection& lhs, const PolygonCollection& rhs)
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

	PolygonCollection Union(const PolygonCollection& lhs, const PolygonCollection& rhs)
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

	PolygonCollection Intersect(const PolygonCollection& lhs, const PolygonCollection& rhs)
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

	PolygonCollection SetDifference(const PolygonCollection& lhs, const PolygonCollection& rhs)
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

	PolygonCollection Multiply(const PolygonCollection& lhs, const PolygonCollection& rhs)
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

	/*	*********************************
			Polygon Collection core functions
		*********************************/
	PolygonCollection PolygonCollection::ApplyMatrix(double a, double b, double c, double d) const
	{
		PolygonCollection ret;
		for (auto p : polygons)
		{
			ret.Add(p.ApplyMatrix(a,b,c,d));
		}

		return ret;
	}

	PolygonCollection PolygonCollection::Transpose() const
	{
		PolygonCollection ret;
		for (auto p : polygons)
		{
			ret.Add(p.Transpose());
		}

		return ret;
	}

	PolygonCollection& PolygonCollection::operator= (const PolygonCollection& rhs)
	{
		polygons.clear();
		for (auto p : rhs.polygons)
		{
			Polygon copy = p;
			polygons.push_back(copy);
		}
		return *this;
	}

	PolygonCollection& PolygonCollection::Add(const Polygon add)
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
}
