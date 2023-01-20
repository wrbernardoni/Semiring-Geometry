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

	cout << endl;
	cout << endl;

	// Ex 2.22 with storage	
	cout << "Example 2.22 with storage\n";
	Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4> bigM;
	for (int i = 0; i < 4; i++)
	{
		bigM(i,i) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int, unsigned int)->Semiring::BooleanSemiring{
			return Semiring::BooleanSemiring(true);
		});
	}
	bigM(0,1) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 1) && (t >= 0) && (d == 10))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});

	bigM(0,2) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 1) && (t >= 0) && (d == 0))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});

	bigM(1,3) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 11) && (t >= 10) && (d == 5))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});

	bigM(2,3) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 6) && (t >= 5) && (d == 0))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});


	Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4> newM;
	newM = Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4>::One() + bigM + bigM * bigM + bigM * bigM * bigM;
	cout << newM(0,3) << endl;

	// Ex 2.22 without storage
	Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4> noStorage;
	
	noStorage(0,1) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 1) && (t >= 0) && (d == 10))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});

	noStorage(0,2) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 1) && (t >= 0) && (d == 0))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});

	noStorage(1,3) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 11) && (t >= 10) && (d == 5))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});

	noStorage(2,3) = Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::BooleanSemiring{
		if ((t <= 6) && (t >= 5) && (d == 0))
			return Semiring::BooleanSemiring(true);
		else
			return Semiring::BooleanSemiring(false);
	});


	cout << "Example 2.22 without storage\n";
	Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4> starNoStorage;
	starNoStorage = Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4>::One() + noStorage + noStorage * noStorage + noStorage * noStorage * noStorage;
	cout << starNoStorage(0,3) << endl;

	return 0;
}