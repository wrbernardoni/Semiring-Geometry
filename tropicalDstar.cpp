#include <iostream>
#include <Semirings.h>

using namespace std;

int main()
{
	cout << "Hello world." << endl;
	cout << Semiring::TropicalSemiring::Zero();
	cout << endl;// << endl;
	cout << Semiring::TropicalSemiring::One();
	cout << endl;// << endl;
	cout << (Semiring::TropicalSemiring() + Semiring::TropicalSemiring::One()) * Semiring::TropicalSemiring(4);
	cout << endl;

	Semiring::Matrix<Semiring::TropicalSemiring,3,3> m;
	m(0,0) = Semiring::TropicalSemiring(4);
	m(1,1) = Semiring::TropicalSemiring(5);
	m(2,2) = Semiring::TropicalSemiring(1);
	m(2,0) = Semiring::TropicalSemiring(1);
	m(1,0) = Semiring::TropicalSemiring(10);
	m(0,1) = Semiring::TropicalSemiring(5);
	m(1,2) = Semiring::TropicalSemiring(100);
	m(2,1) = Semiring::TropicalSemiring(3);
	cout << m;
	cout << endl;
	cout << Semiring::TropicalSemiring(0)*(Semiring::Matrix<Semiring::TropicalSemiring,3,3>::One() + m + m * m);
	cout << endl;
	return 0;
}