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
	return 0;
}