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
	cout << newM(0,0) << endl;

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
	cout << starNoStorage(0,0) << endl;
	cout << endl;

	// Semiring::HopLimitedSemiring<3> n(1);
	// cout << n << endl;
	// cout << n * n << endl;
	// cout << n * n * n << endl;
	// cout << n * n * n * n << endl;
	// cout << endl;



	Semiring::Matrix<Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>,5,5> FourSteps;
	for (int i = 0; i < 4; i++)
	{
		FourSteps(i,i) = Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<5>{
			if (d <= 1)
				return Semiring::HopLimitedSemiring<5>(1);
			else
				return Semiring::HopLimitedSemiring<5>::Zero();
		});
	}

	FourSteps(0,1) = Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<5>{
		if ((t <= 2) && (t >= 0) && (d == 2))
			return Semiring::HopLimitedSemiring<5>(1);
		else
			return Semiring::HopLimitedSemiring<5>::Zero();
	});

	FourSteps(1,2) = Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<5>{
		if ((t <= 3) && (t >= 3) && (d == 2))
			return Semiring::HopLimitedSemiring<5>(1);
		else
			return Semiring::HopLimitedSemiring<5>::Zero();
	});

	FourSteps(2,3) = Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<5>{
		if ((t <= 10) && (t >= 6) && (d == 2))
			return Semiring::HopLimitedSemiring<5>(1);
		else
			return Semiring::HopLimitedSemiring<5>::Zero();
	});

	FourSteps(0,4) = Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<5>{
		if ((t >= 0) && (t <= 10) && (d == 3))
			return Semiring::HopLimitedSemiring<5>(1);
		else
			return Semiring::HopLimitedSemiring<5>::Zero();
	});

	FourSteps(4,3) = Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<5>{
		if ((t >= 8) && (t <= 10) && (d == 1))
			return Semiring::HopLimitedSemiring<5>(1);
		else
			return Semiring::HopLimitedSemiring<5>::Zero();
	});

	Semiring::Matrix<Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>,5,5> pK = Semiring::Matrix<Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>,5,5>::One() + FourSteps;
	for (int i = 0; i < 10; i++)
	{
		pK = pK * pK;
	}

	cout << "1->1" << endl;
	cout << pK(0,0)  << endl;
	cout << "1->2" << endl;
	cout << pK(0,1)  << endl;
	cout << "1->3" << endl;
	cout << pK(0,2)  << endl;
	cout << "1->4" << endl;
	cout << pK(0,3)  << endl;
	cout << "1->5" << endl;
	cout << pK(0,4)  << endl;

	auto PS = Semiring::PartialStar<Semiring::Matrix<Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>,5,5>>(pK, 30);
	cout << PS << endl;

	// Semiring::Matrix<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>,4,4>
	auto exMin = Semiring::MinimalPaths<Semiring::ContactSemiring<Semiring::BooleanSemiring, 16>, 4>(bigM, 30);
	cout << exMin;

	cout << endl << "Centralities\t(in)\t(out)\t(store)\t(avg):" << endl;
	auto exinCent = Semiring::IndegreeOptimalCentrality(exMin);
	auto exoutCent = Semiring::OutdegreeOptimalCentrality(exMin);
	auto estoreCent = Semiring::StorageCentrality(exMin);
	for (int i = 0; i < 4; i++)
	{
		cout << "["<< i << "]\t("<< exinCent[i] << "), (" << exoutCent[i] << "), (" << estoreCent[i] << "), (" << (exinCent[i] + exoutCent[i] + estoreCent[i])/3.0 << ")" << endl;
	}

	// cout << std::endl << std::endl;
	// cout << Semiring::MinimalPaths<Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>, 30>>(FourSteps, 30);

	// cout << Semiring::ContactSemiring<Semiring::HopLimitedSemiring<5>,30>::One() * pK << endl;

	// Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<3>> x;// = Semiring::FreeIdempotentSemiring<Semiring::HopLimitedSemiring<3>>::One();
	// x.insert(Semiring::FreeMonoid<3>());
	// x.insert(Semiring::FreeMonoid<3>(1));
	// x.insert(Semiring::FreeMonoid<3>(2));
	// cout << x << endl;
	// cout << x * x << endl;
	// cout << x * x * x << endl;
	// cout << x * x * x * x << endl;

	// auto y = x;
	// for (int i = 0; i < 3; i++)
	// {
	// 	y = y * x;
	// }
	// cout << y << endl;
	// cout << x;


	cout << endl << endl;
	
	const int NN = 20;
	const int timeLimit = 300;
	const int hopLimit = 10;
	const int storageTime = 10;
	int linkNum = 300;
	cout << NN << " x " << NN << " test" << endl;
	Semiring::Matrix<Semiring::ContactSemiring<Semiring::HopLimitedSemiring<hopLimit>, timeLimit>,NN,NN> bigBoi([](unsigned int i, unsigned int j)->Semiring::ContactSemiring<Semiring::HopLimitedSemiring<hopLimit>, timeLimit>{
		if (i == j)
		{
			return Semiring::ContactSemiring<Semiring::HopLimitedSemiring<hopLimit>, timeLimit>([](unsigned int t, unsigned int d)->Semiring::HopLimitedSemiring<hopLimit>
			{
				if (d == 0)
				{
					return Semiring::HopLimitedSemiring<hopLimit>::Zero();//(0);
				}
				else //if (d <= storageTime)
				{
					return Semiring::HopLimitedSemiring<hopLimit>(d/storageTime + 1);
				}
				// else
				// {
				// 	return Semiring::HopLimitedSemiring<hopLimit>::Zero();
				// }
			});
		}
		else
		{
			return Semiring::ContactSemiring<Semiring::HopLimitedSemiring<hopLimit>, timeLimit>::Zero();
		}
	});
	srand (time(NULL));
	for (int  i = 0; i < linkNum; i++)
	{
		int source = rand() % NN;
		int target = rand() % NN;
		int timeStart = rand() % timeLimit;
		if ((source == target) || ((timeLimit - timeStart) < 11))
		{
			linkNum++;
			continue;
		}
		int timeEnd = timeStart + rand() % (timeLimit - 10 - timeStart);
		int delay = rand() % (timeLimit - timeEnd);

		cout << "Link (" << source << "," << target << ") on [" << timeStart << "," << timeEnd << "], owlt: " << delay << endl;
		for (int t = timeStart; t < timeEnd; t++)
		{
			bigBoi(source, target)(t, delay) = bigBoi(source, target)(t, delay) + Semiring::HopLimitedSemiring<hopLimit>(1);
		}
	}
	cout << "Matrix generated" << endl;
	cout << "Beginning shortest path generation" << endl;
	cout << endl << "-----------" << endl;
	auto Opt = Semiring::MinimalPaths(bigBoi, 2 * hopLimit, Semiring::ContactSemiring<Semiring::HopLimitedSemiring<hopLimit>, timeLimit>([](unsigned int t, unsigned int d) -> Semiring::HopLimitedSemiring<hopLimit> {
		if ((t == 0) && (d == 0))
		{
			return Semiring::HopLimitedSemiring<hopLimit>(0);
		}
		else
		{
			return Semiring::HopLimitedSemiring<hopLimit>::Zero();
		}
	}));
	cout << Opt;
	cout << endl << "Shortest Paths found." << endl;

	cout << endl << "Centralities\t(in)\t(out)\t(store)\t(avg):" << endl;
	auto inCent = Semiring::IndegreeOptimalCentrality(Opt);
	auto outCent = Semiring::OutdegreeOptimalCentrality(Opt);
	auto storeCent = Semiring::StorageCentrality(Opt);
	for (int i = 0; i < NN; i++)
	{
		cout << "["<< i << "]\t("<< inCent[i] << "), (" << outCent[i] << "), (" << storeCent[i] << "), (" << (inCent[i] + outCent[i] + storeCent[i])/3.0 << ")" << endl;
	}
	
	/*
	cout << "Testing minimal dependents on [1][2][3][4][5]" << endl;
	cout << "True Dependents: [1], [2][3], [3][4][5]" << endl;
	std::unordered_set<Semiring::FreeMonoid<5>, Semiring::StreamHash<Semiring::FreeMonoid<5>>> top;
	top.insert(Semiring::FreeMonoid<5>(1));
	top.insert(Semiring::FreeMonoid<5>(2));
	top.insert(Semiring::FreeMonoid<5>(3));
	top.insert(Semiring::FreeMonoid<5>(4));
	top.insert(Semiring::FreeMonoid<5>(5));
	auto test = [](std::unordered_set<Semiring::FreeMonoid<5>, Semiring::StreamHash<Semiring::FreeMonoid<5>>> set)->bool
	{
		if (set.count(Semiring::FreeMonoid<5>(1)) != 0)
		{
			return true;
		}
		else if ((set.count(Semiring::FreeMonoid<5>(2)) != 0) && (set.count(Semiring::FreeMonoid<5>(3)) != 0))
		{
			return true;
		}
		else if ((set.count(Semiring::FreeMonoid<5>(3)) != 0)&&(set.count(Semiring::FreeMonoid<5>(4)) != 0)&&(set.count(Semiring::FreeMonoid<5>(5)) != 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	auto mDep = MinimalSubsets(top, test);
	cout << "Dependents found:" << endl;
	for (auto it = mDep.begin(); it != mDep.end(); it++)
	{
		for (auto it2 = (*it).begin(); it2 != (*it).end(); it2++)
		{
			cout << (*it2);
		}
		cout << endl;
	}
	*/


	return 0;
}