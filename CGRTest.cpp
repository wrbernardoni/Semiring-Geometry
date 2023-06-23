#include <iostream>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>

#include <Semirings.h>
#include <Monoids.h>
#include <ShortestPath.h>

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
	ex(0,2) = CGRSemiring(Contact(0,1,0));
	ex(1,3) = CGRSemiring(Contact(10,11,5));
	ex(2,3) = CGRSemiring(Contact(5,6,0));

	cout << ex << endl;

	auto run = ex;

	for (int i = 1; i < 10; i++)
	{
		cout << "[" << i << "]th power of ex from A->D: " << endl << run(0,3) << endl << endl;
		run = run * ex;
	}

	cout << (run(0,3) <= CGRSemiring::Storage()) << endl;
	cout << (ex <= run) << endl;
	cout << (run <= ex) << endl;
	cout << (run == (run * ex)) << endl;
	// cout << (run(0,3) <= run(0,3)) << endl;
	// cout << (CGRSemiring::Storage() <= run(0,3)) << endl;

	const int NN = 3;
	Matrix<CGRSemiring,NN,NN> testCase = Matrix<CGRSemiring,NN,NN>::One();
	for (int i = 0; i < NN; i++)
	{
		// if (i == 0)
		// 	continue;
		testCase(i,i) = CGRSemiring::Storage();
	}

	testCase(0,1) = testCase(0,1) + CGRSemiring(Contact(2,3,0.2));
	testCase(0,1) = testCase(0,1) + CGRSemiring(Contact(4,6,0.2));

	testCase(0,2) = testCase(0,2) + CGRSemiring(Contact(2,5,0.4));

	testCase(1,0) = testCase(1,0) + CGRSemiring(Contact(1,7,0.2));

	testCase(1,2) = testCase(1,2) + CGRSemiring(Contact(2,3,0.3));
	testCase(1,2) = testCase(1,2) + CGRSemiring(Contact(4,6,0.3));
	testCase(1,2) = testCase(1,2) + CGRSemiring(Contact(7,8,0.3));

	testCase(2,0) = testCase(2,0) + CGRSemiring(Contact(5,9,0.4));

	testCase(2,1) = testCase(2,1) + CGRSemiring(Contact(1,5,0.3));

	auto r = testCase;
	cout << testCase << endl;
	for (int i = 1; i < 10; i++)
	{
		cout << "[" << i << "]th power of test from A->C: " << endl << r(1,2) << endl << endl;
		r = r * testCase;
	}
	cout << r << endl;

	auto Opt = MinimalPaths(testCase, 100);
	cout << Opt;

	cout << endl << "Graph Connectivity: " << Connectivity(Opt) * 100.0 << "%" << endl;
	cout << endl << "Centralities\t(in)\t(out)\t(store)\t(avg):" << endl;
	auto inCent1 = Semiring::IndegreeOptimalCentrality(Opt);
	auto outCent1 = Semiring::OutdegreeOptimalCentrality(Opt);
	auto storeCent1 = Semiring::StorageCentrality(Opt);
	for (int i = 0; i < NN; i++)
	{
		cout << "["<< i << "]\t("<< inCent1[i] << "), (" << outCent1[i] << "), (" << storeCent1[i] << "), (" << (inCent1[i] + outCent1[i] + storeCent1[i])/3.0 << ")" << endl;
	}

	const int NN2 = 15;
	const int timeLimit = 86400;
	const int minContactLength = 60;
	const int maxContactTimeIC = 3600;
	const int maxContactTimeOC = 480;
	// const int storageTime = 10;
	const int maxDelay = 120;

	const int outerClusterDelay = 750;
	const int outerClusterSpread = 580;

	const int intraClusterDelay = 10;
	const int intraClusterSpread = 8;

	const int ClusterBoundary = 11;

	const int STEPS = 20;
	int linkNum = 300;
	cout << NN2 << " x " << NN2 << " test" << endl;
	Semiring::Matrix<CGRSemiring,NN2+2,NN2+2> bigBoi([](unsigned int i, unsigned int j)->CGRSemiring{
		if ((i == j))// && ((i == 0)||(i == NN2+1)||((i%3)==0)))
		{
			return CGRSemiring::Storage();
		}
		else
		{
			return CGRSemiring::Zero();
		}
	});

	for (int i = 1; i < ClusterBoundary; i++)
	{
		// if ((i%2)==0)
		// {
			bigBoi(0,i) = CGRSemiring(Contact(0,0,intraClusterDelay,true,true));
			bigBoi(i,0) = CGRSemiring(Contact(0,0,intraClusterDelay,true,true));
		// }
	}

	for (int i = ClusterBoundary; i <= NN2; i++)
	{
		// if ((i%2) == 0)
		// {
			bigBoi(i,NN2+1) = CGRSemiring(Contact(0,0,intraClusterDelay,true,true));
			bigBoi(NN2+1,i) = CGRSemiring(Contact(0,0,intraClusterDelay,true,true));
		// }
	}

	srand (time(NULL));
	for (int  i = 0; i < linkNum; i++)
	{
		int source = rand() % NN2 + 1;
		int target = rand() % NN2 + 1;
		int timeStart = rand() % timeLimit;
		if ((source == target))
		{
			linkNum++;
			continue;
		}

		int timeEnd = 0;//timeStart + rand() % (maxContactTime);
		int delay = 0;//rand() % (maxDelay);

		if (((source < ClusterBoundary) && (target < ClusterBoundary))
			|| ((source >= ClusterBoundary) && (target >= ClusterBoundary)))
		{
			timeEnd = timeStart + minContactLength + rand() % (maxContactTimeIC - minContactLength);
			delay = intraClusterDelay + rand() % (2 * intraClusterSpread) - intraClusterSpread;
		}
		else
		{
			timeEnd = timeStart + minContactLength + rand() % (maxContactTimeOC - minContactLength);
			delay = outerClusterDelay + rand() % (2 * outerClusterSpread) - outerClusterSpread;
		}

		cout << "Link (" << source << "," << target << ") on [" << timeStart << "," << timeEnd << "], owlt: " << delay << endl;
		for (int t = timeStart; t < timeEnd; t++)
		{
			bigBoi(source, target) = bigBoi(source, target) + CGRSemiring(Contact(timeStart, timeEnd, delay));
		}
	}

	auto o = MinimalPaths(bigBoi, STEPS);
	cout << o << endl;

	cout << endl << "Graph Connectivity: " << Connectivity(o) * 100.0 << "%" << endl;

	cout << endl << "Centralities\t(in)\t(out)\t(store)\t(avg):" << endl;
	auto inCent = Semiring::IndegreeOptimalCentrality(o);
	auto outCent = Semiring::OutdegreeOptimalCentrality(o);
	auto storeCent = Semiring::StorageCentrality(o);
	for (int i = 0; i < NN2 + 2; i++)
	{
		cout << "["<< i << "]\t("<< inCent[i] << "), (" << outCent[i] << "), (" << storeCent[i] << "), (" << (inCent[i] + outCent[i] + storeCent[i])/3.0 << ")" << endl;
	}

	cout << endl << "Earth to mars opt path: " << o(0,NN2+1) << endl;
	cout << endl << "Cost: " << PartialStar(bigBoi,STEPS)(0,NN2+1) << endl;


	// CGRSemiring ta(CSC_Contact(Contact(0,100,0),Contact(50,80,10)));
	// cout << ta << endl;
	// auto ra = ta;
	// for (int i = 1; i < 30; i++)
	// {
	// 	cout << "[" << i << "]th power of ta " << endl << ra << endl << endl;
	// 	ra = ra * ta;
	// }

	// cout << Contact(50,50,40) * Contact(0,80,0) << endl;
	// cout << CSC_Contact(Contact(0,50,0), Contact(50,50,40)) * CSC_Contact(Contact(0,80,0), Contact()) << endl;

	//{[(1,0) (0,2)], [(1,1) (1,2) (2,2)], [(1,0) (0,0) (0,2)]
	// auto a = testCase(1,0) * testCase (0,2);
	// auto b = testCase(1,1) * testCase(1,2) * testCase(2,2);
	// auto c = testCase(1,0) * testCase(0,0) * testCase(0,2);

	// cout << a << endl;
	// cout << b << endl;
	// cout << c << endl;
	// cout << endl << endl;

	// cout << (a >= c) << endl;
	// cout << ((a + b + c) == (c + b)) << endl;

	// cout << endl;
	// cout << "Done!" << endl;
	// cout << (testCase(0,0) <= CGRSemiring::One()) << endl;
	// cout << (testCase(0,0) >= CGRSemiring::One()) << endl;
	// cout << (testCase(0,0) == CGRSemiring::One()) << endl;

	// cout << (CGRSemiring::One() == CGRSemiring::Zero()) << endl;

	// cout << testCase(1,0) * testCase(0,2) << endl;
	// cout << testCase(1,0) * testCase(0,0) * testCase(0,2) << endl;
	// cout << ((testCase(1,0) * testCase(0,2) ) >= (testCase(1,0) * testCase(0,0) * testCase(0,2))) << endl;
	// cout << testCase(1,0) * testCase(0,0) * testCase(0,2) + testCase(1,0)  * testCase(0,2) << endl;
	// cout << (CSC_Contact(Contact(1,4.8,0), Contact(1.8, 4.8, 0.6)) >= CSC_Contact(Contact(1.8,4.8,0.6))) << endl;
	// cout << (CSC_Contact(Contact(1,4.8,0), Contact(1.8, 4.8, 0.6)) <= CSC_Contact(Contact(1.8,4.8,0.6))) << endl;
	// cout << (CSC_Contact(Contact(1.8,4.8,0.6)) >= CSC_Contact(Contact(1,4.8,0), Contact(1.8, 4.8, 0.6))) << endl;
	// cout << (CSC_Contact(Contact(1.8,4.8,0.6)) <= CSC_Contact(Contact(1,4.8,0), Contact(1.8, 4.8, 0.6))) << endl;
	// auto b =(testCase(1,0)*testCase(0,2));
	// cout << b.Add(CSC_Contact(Contact(1,4.8,0), Contact(1.8, 4.8, 0.6))) << endl;

	return 0;
}
