#include <iostream>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>

#include <Polygons.h>

using namespace std;
using namespace Semiring;
using namespace Polyhedral;

int main()
{
	cout << "Hello world!" << endl;

	Polygon emptyPoly;
	cout << "Empty polygon (should be []): " << emptyPoly << endl;

	PolygonCollection emptyCollection;
	cout << "Empty polygon collection (should be {}): " << emptyCollection << endl;

	cout << "Adding empty polygon to empty collection (should still be empty): " << emptyCollection.Add(emptyPoly) << endl;
	cout << "Is the empty polygon a subset of the empty collection (should be 1): " << SubsetEq(emptyPoly, emptyCollection) << endl;
	cout << "Is the empty collection a subset of the empty polygon (should be 1): " << SubsetEq(emptyCollection, emptyPoly) << endl;

	Polygon uBox = Polygon::UnitBox();
	Polygon hPlane = Polygon::HalfPlane(Point(0,0.5), Point(1,0.5));
	cout << "Unit box: " << uBox << endl;
	cout << "Vertical halfplane offset by 1/2: " << hPlane << endl;
	cout << "Union of the above: " << Union(uBox, hPlane) << endl;
	cout << "Intersection of the above: " << Intersect(uBox, hPlane) << endl;
	cout << "Box minus halfplane: " << SetDifference(uBox, hPlane) << endl;
	cout << "Halfplane minus box: " << SetDifference(hPlane, uBox) << endl;
	return 0;
}