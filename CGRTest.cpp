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
	cout << endl;

	cout << CSC_Contact(Contact(0,100,10)) * CSC_Contact() * CSC_Contact(Contact(8,13,2)) * CSC_Contact() * CSC_Contact(Contact(0,100,10)) << endl;
	cout << CSC_Contact() * CSC_Contact(Contact(0,100,10)) * CSC_Contact() * CSC_Contact(Contact(8,13,2)) * CSC_Contact() * CSC_Contact(Contact(0,100,10)) * CSC_Contact()<< endl;
	
	cout << endl;
	cout << endl;
	cout << "Example 2.39:"<<endl;
	cout << "Top: " << CSC_Contact() * CSC_Contact(Contact(0,1,10)) * CSC_Contact() * CSC_Contact(Contact(10,11,5)) * CSC_Contact() << endl;
	cout << "Bottom: " << CSC_Contact() * CSC_Contact(Contact(0,1,0)) * CSC_Contact() * CSC_Contact(Contact(5,6,0)) * CSC_Contact() << endl;
	cout << "Bottom contains top? " << (CSC_Contact() * CSC_Contact(Contact(0,1,0)) * CSC_Contact() * CSC_Contact(Contact(5,6,0)) * CSC_Contact() <= CSC_Contact() * CSC_Contact(Contact(0,1,10)) * CSC_Contact() * CSC_Contact(Contact(10,11,5)) * CSC_Contact()) << endl;
	cout << "Donezo" << endl;

	cout << endl;
	cout << endl;
	auto c1 = CSC_Contact() * CSC_Contact(Contact(0,10,3)) * CSC_Contact();
	auto c2 = CSC_Contact() * CSC_Contact(Contact(0,100,2)) * CSC_Contact();
	cout << c1 << " " << c2 << endl;
	cout << (CSC_Contact(Contact(0,10,3)) <= c1) << " " << (c1 <= CSC_Contact(Contact(0,10,3))) << endl;
	cout << (CSC_Contact(Contact(0,10,3)) <= c2) << " " << (c2 <= CSC_Contact(Contact(0,10,3))) << endl;
	cout << (c2 <= c1) << endl;
	cout << (c1 <= c2) << endl;
	cout << (c1 <= c1) << endl;
	cout << (c2 <= c2) << endl;

	auto c3 = c1 * CSC_Contact(Contact(0,5,2));
	cout << c3 << endl;
	cout << (c2 <= c3) << " " << (c3 <= c2) << endl;
	
	cout << endl;
	cout << endl;
	cout << "CGR Semiring!" << endl;
	cout << "Zero: " << CGRSemiring::Zero() << endl;
	cout << "One: " << CGRSemiring::One() << endl;
	cout << "Storage: " << CGRSemiring::Storage() << endl;
	cout << "Storage + One: " << (CGRSemiring::Storage() + CGRSemiring::One()) << endl;
	cout << "Storage * One: " << (CGRSemiring::Storage() * CGRSemiring::One()) << endl;
	cout << "One + Storage: " << (CGRSemiring::One() + CGRSemiring::Storage()) << endl;
	cout << "One * Storage: " << (CGRSemiring::One() * CGRSemiring::Storage()) << endl;

	cout << "Example 2.39 matrix: " << endl;
	Matrix<CGRSemiring,4,4> ex;
	for (int i = 0; i < 4; i++)
	{
		ex(i,i) = CGRSemiring::Storage();
	}

	ex(0,1) = CGRSemiring(Contact(0,1,10));
	ex(0,2) = CGRSemiring(Contact(1,3,0));
	ex(1,3) = CGRSemiring(Contact(10,11,5));
	ex(2,3) = CGRSemiring(Contact(5,6,0));

	cout << ex << endl;

	auto run = ex;

	for (int i = 1; i < 10; i++)
	{
		cout << "[" << i << "]th power of ex from A->D: " << endl << run(0,3) << endl << endl;
		run = run * ex;
	}

	return 0;
}
