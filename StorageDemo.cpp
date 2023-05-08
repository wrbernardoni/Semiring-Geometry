#include <iostream>
#include <Semirings.h>
#include <Monoids.h>
#include <ShortestPath.h>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>

using namespace std;

int main()
{
	Semiring::Matrix<Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>,4,4> thruEx;
	Semiring::Matrix<Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>,4,4> oneShotEx;

	for (int i = 0; i < 4; i++)
	{
		thruEx(i,i) = Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>([](unsigned int, unsigned int)->Semiring::MaxMinSemiring{
			return Semiring::MaxMinSemiring::One();
		});
	}

	for (int i = 0; i < 4; i++)
	{
		oneShotEx(i,i) = Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>::One();
	}


	oneShotEx(0,1) = thruEx(0,1) = Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::MaxMinSemiring{
		if ((t <= 5) && (t >= 0) && (d == 0))
			return Semiring::MaxMinSemiring(5 - t);
		else
			return Semiring::MaxMinSemiring::Zero();
	});

	oneShotEx(0,2) = thruEx(0,2) = Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::MaxMinSemiring{
		if ((t <= 5) && (t >= 0) && (d == 0))
			return Semiring::MaxMinSemiring(5 - t);
		else
			return Semiring::MaxMinSemiring::Zero();
	});

	oneShotEx(1,3) = thruEx(1,3) = Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::MaxMinSemiring{
		if ((t <= 7) && (t >= 3) && (d == 0))
			return Semiring::MaxMinSemiring(7 - t);
		else
			return Semiring::MaxMinSemiring::Zero();
	});

	oneShotEx(2,3) = thruEx(2,3) = Semiring::ContactSemiring<Semiring::MaxMinSemiring, 16>([](unsigned int t, unsigned int d)->Semiring::MaxMinSemiring{
		if ((t <= 11) && (t >= 6) && (d == 0))
			return Semiring::MaxMinSemiring(11 - t);
		else
			return Semiring::MaxMinSemiring::Zero();
	});

	// cout << ex(0,3) << endl;
	// auto star = ex * ex * ex * ex;
	// cout << star(0,3) << endl;
	auto starOS = Semiring::PartialStar(oneShotEx,10);
	auto starThru = Semiring::PartialStar(thruEx,10);
	cout << "No storage 0->3:" << endl << starOS(0,3) << endl << endl;


	cout << "With storage 0->3:" << endl << starThru(0,3) << endl << endl;
}