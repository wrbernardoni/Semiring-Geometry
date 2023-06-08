#include <Polygons.h>

using namespace Semiring;
using namespace Semiring::Polyhedral;


/*
		Polygon utility functions
*/
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


/*
			Polygon core functions
*/
Polygon::Polygon()
{
	
}

Polygon& Polygon::operator= (const Polygon& rhs)
{
	// TODO copy polygon
	return *this;
}