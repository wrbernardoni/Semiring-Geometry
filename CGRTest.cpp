#include <iostream>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>

#include <Semirings.h>

using namespace std;
using namespace Semiring;

int main()
{
	// cout << "Contact? " << Contact(0,10,2) * Contact(2,3,1) << endl;
	// cout << Contact() << endl;
	// cout << Contact(5,15,12) << endl;
	// cout << Contact(5,15,12) * Contact() << endl;
	cout << "Storage? " << CSC_Contact(Contact(0,100,1), Contact(3,6,1)) * CSC_Contact(Contact(5,10,1), Contact(10,12,1)) << endl;
	
	cout << "Top:" << CSC_Contact(Contact(0,2,1)) * CSC_Contact(Contact(),Contact()) * CSC_Contact(Contact(4,7,1))* CSC_Contact(Contact(),Contact()) * CSC_Contact(Contact(5,15,1)) * CSC_Contact(Contact(),Contact());
	cout << endl;
	cout << "Bottom:" << CSC_Contact(Contact(6,10,1)) * CSC_Contact(Contact(),Contact()) * CSC_Contact(Contact(8,9,1))* CSC_Contact(Contact(),Contact()) * CSC_Contact(Contact(5,15,1)) * CSC_Contact(Contact(),Contact());
	cout << endl;
	cout << CSC_Contact(Contact(6,10,1)) * CSC_Contact(Contact(),Contact()) * CSC_Contact(Contact(8,9,1)) << endl;
	cout << endl;
	cout << CSC_Contact(Contact(6,10,1), Contact(8,9,1));
	cout << endl;
	cout << endl;
	cout << CSC_Contact() * CSC_Contact(Contact(0,10,3)) * CSC_Contact();
	cout << endl;
	cout << "Donezo" << endl;
	return 0;
}