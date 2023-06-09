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
	cout << "Empty polygon: " << emptyPoly << endl;

	PolygonCollection emptyCollection;
	cout << "Empty polygon collection: " << emptyCollection << endl;
	return 0;
}