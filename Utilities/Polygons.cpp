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


/*
			Polygon core functions
*/
Polygon::Polygon()
{
	boundingBox = 1.0;
}

Polygon& Polygon::operator= (const Polygon& rhs)
{
	// TODO copy polygon
	return *this;
}

void Polygon::AdjustBoundingBox(long double nbb)
{
	if (nbb == boundingBox)
		return;

	// TODO adjust the bounding box on the polygon
}

Polygon Polygon::Transpose() const
{
	//TODO return the transpose of this polygon

	return Polygon();
}

Polygon Polygon::ApplyMatrix(double a, double b, double c, double d) const
{
	// TODO apply the matrix to this polygon and return the result

	return Polygon();
}