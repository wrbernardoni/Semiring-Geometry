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
	return 0;
}