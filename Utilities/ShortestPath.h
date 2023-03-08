#ifndef SHORTEST_PATH_H_
#define SHORTEST_PATH_H_

#include <Semirings.h>

#include <iostream>

#include <vector>
#include <unordered_map>


#include "UtilitiesDefinitions.h"

namespace Semiring
{
	template<typename T> 
	T PartialStar(T x, unsigned int n)
	{
		if (n == 0)
			return T::One();
		T d = x + T::One();
		T pD = d;
		for (unsigned int i = 1; i <= n; i++)
		{
			T nD = pD * d;
			if (nD == pD)
			{
				return nD;
			}

			pD = nD;
		}
		return pD;
	}

	template<typename CostType, unsigned int N>
	Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> MinimalPaths(Semiring::Matrix<CostType,N,N> costMatrix, unsigned int max_steps, CostType costPrepend = CostType::One())
	{
		#ifdef VERBOSE
			std::cout << "\tSetting up path matrix" << std::endl;
		#endif
		auto dispL = [](unsigned int x)->std::string{
			if (x == 0)
				return "-";
			std::string s = "(";
			unsigned int r = (x-1)/N;
			unsigned int c = (x-1)%N;
			s += std::to_string(r);
			s += ",";
			s += std::to_string(c);
			s += ")";
			return s;
		};

		Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> pathMatrix([&costMatrix, &dispL](unsigned int t, unsigned int d)->Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>> {
			auto e = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>());
			if (costMatrix(t,d) != CostType::Zero())
			{
				auto n = Semiring::FreeMonoid<N * N>(t * N + d + 1);
				n.setLabel(dispL);
				e = e + Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>(n));
			}

			if (t == d)
			{
				auto n = Semiring::FreeMonoid<N * N>::One();
				n.setLabel(dispL);
				e = e + Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>(n));
			}

			return e;
		});

		Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>,N,N> pBundle;
		for (int t = 0; t < N; t++)
		{
			for (int d = 0; d < N; d++)
			{
				auto opSet = pathMatrix(t,d).getSet();
				std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> aPaths;
				for (auto itr = opSet.begin(); itr != opSet.end(); itr++)
				{
					aPaths = Semiring::Union(aPaths, itr->getSet());
				}

				pBundle(t,d) = aPaths;
			}
		}

		#ifdef VERBOSE
			std::cout << "\tSetting up step matrices" << std::endl;
		#endif
		Semiring::Matrix<CostType,N,N> costStep = costMatrix + Semiring::Matrix<CostType,N,N>::One();
		// pathMatrix =  pathMatrix + Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N>::One();
		auto pathStep = pBundle;
		auto cMatrix = costPrepend * costStep;
		#ifdef VERBOSE
			std::cout << "\tMatrices created" << std::endl;
		#endif

		Semiring::Matrix<CostType,N,N> pCMatrix;

		for (int i = 1; i <= max_steps; i++)
		{
			// Reduce paths
			#ifdef VERBOSE
				std::cout << "\tReducing path possibilities to minimal bundles" << std::endl;
			#endif
			for (int t = 0; t < N; t++)
			{
				for (int d = 0; d < N; d++)
				{
					// if ((cMatrix(t,d) == pCMatrix(t,d)) && (i > 1))
					// 	continue;
					
					if (cMatrix(t,d) == CostType::Zero())
					{
						// #ifdef VERBOSE
						// 	std::cout << "\t\t\tNo available path" << std::endl;
						// #endif
						pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>::Zero();
						continue;
					}

					#ifdef VERBOSE
						std::cout << "\t\t Reducing entry ("<< t << "," << d << ")" << std::endl;
					#endif
					std::unordered_set<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>, StreamHash<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>> availPaths;
					std::unordered_map<Semiring::FreeMonoid<N * N>, int, StreamHash<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>> pMap;

					std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> aPaths = pBundle(t,d).getSet();;

					std::vector<Semiring::FreeMonoid<N * N>> bases;
					std::vector<CostType> baseCost;
					std::unordered_set<Semiring::FreeMonoid<0>, Semiring::StreamHash<Semiring::FreeMonoid<0>>> bses;
					for (auto itr = aPaths.begin(); itr != aPaths.end(); itr++)
					{

						CostType pathCost = costPrepend;
						for (int step = 0; step < (*itr).size(); step++)
						{
							if ((*itr).at(step) == 0)
								continue;
							int edge = (*itr).at(step) - 1;
							int r = edge/N;
							int c = edge%N;
							pathCost = pathCost * costMatrix(r,c);
						}

						if (pathCost != CostType::Zero())
						{
							bases.push_back((*itr));
							baseCost.push_back(pathCost);
							bses.insert(Semiring::FreeMonoid<0>(bases.size()));
							pMap[(*itr)] = bases.size();
						}
					}
					std::vector<std::unordered_set<Semiring::FreeMonoid<0>, Semiring::StreamHash<Semiring::FreeMonoid<0>>>> seeds;

					// if ((cMatrix(t,d) == pCMatrix(t,d)) && (i > 1))
					// {
					// 	#ifdef VERBOSE
					// 		std::cout << "\t\t\tCost unchanged, seeding search with previous minimal paths." << std::endl;
					// 	#endif

					// 	auto pMinimal = pathMatrix(t,d).getSet();
					// 	for (auto mSet = pMinimal.begin(); mSet != pMinimal.end(); mSet++)
					// 	{
					// 		std::unordered_set<Semiring::FreeMonoid<0>, Semiring::StreamHash<Semiring::FreeMonoid<0>>> pSet;
					// 		bool good = true;
					// 		auto pathBundle = (*mSet).getSet();
					// 		for (auto p = pathBundle.begin(); p != pathBundle.end(); p++)
					// 		{
					// 			if (pMap.count((*p)) != 0)
					// 			{
					// 				pSet.insert(Semiring::FreeMonoid<0>(pMap[(*p)]));
					// 			}
					// 			else
					// 			{
					// 				good = false;
					// 				#ifdef VERBOSE
					// 					std::cout << "\t\t\t\tPrevious minimal path missing???" << std::endl;
					// 				#endif
					// 				break;
					// 			}
					// 		}

					// 		if (good)
					// 		{
					// 			seeds.push_back(pSet);
					// 		}
					// 	}

					// 	#ifdef VERBOSE
					// 		std::cout << "\t\t\t\tSeeding with " << seeds.size() << " minimal bundles." << std::endl;
					// 	#endif
					// }

					if (i > 1)
					{
						#ifdef VERBOSE
							std::cout << "\t\t\tSeeding with minimal products." << std::endl;
						#endif
						
						Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>> minimalSeeds;
						for (int j = 0; j < N; j++)
						{
							auto minimalJoins = (pathMatrix(t,j)).getSet();

							auto runningMinimal = minimalSeeds.getSet();
							Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>> newMinSeed;

							for (auto it1 = minimalJoins.begin(); it1 != minimalJoins.end(); it1++)
							{
								auto newP = ((*it1)*pathStep(j,d)).getSet();
								Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>> runningLink;

								for (auto p = newP.begin(); p != newP.end(); p++)
								{
									if (pMap.count((*p)) != 0)
									{
										runningLink.insert(*p);
									}
								}

								if (runningMinimal.size() != 0)
								{
									for (auto it2 = runningMinimal.begin(); it2 != runningMinimal.end(); it2++)
									{
										newMinSeed.insert((runningLink) + (*it2));
									}
								}
								else if (runningLink.getSet().size() != 0)
								{
									newMinSeed.insert((runningLink));
								}
							}
							if (newMinSeed.getSet().size() != 0)
								minimalSeeds = newMinSeed;
						}

						if ((cMatrix(t,d) == pCMatrix(t,d)))
						{
							#ifdef VERBOSE
								std::cout << "\t\t\t\tCost unchanged, seeding search with previous minimal paths." << std::endl;
							#endif
							minimalSeeds = minimalSeeds + pathMatrix(t,d);
						}

						auto mSS = minimalSeeds.getSet();

						for (auto mSet = mSS.begin(); mSet != mSS.end(); mSet++)
						{
							std::unordered_set<Semiring::FreeMonoid<0>, Semiring::StreamHash<Semiring::FreeMonoid<0>>> pSet;
							bool good = true;
							auto pathBundle = (*mSet).getSet();
							for (auto p = pathBundle.begin(); p != pathBundle.end(); p++)
							{
								if (pMap.count((*p)) != 0)
								{
									pSet.insert(Semiring::FreeMonoid<0>(pMap[(*p)]));
								}
								else
								{
									#ifdef VERBOSE
										std::cout << "\t\t\t\tPrevious minimal path missing???" << std::endl;
									#endif

									good = false;
									break;
								}
							}

							if (good)
							{
								seeds.push_back(pSet);
							}
						}

						#ifdef VERBOSE
							std::cout << "\t\t\t\tSeeding with " << seeds.size() << " total seeds." << std::endl;
						#endif
					}

					auto optCost = cMatrix(t,d);

					#ifdef VERBOSE
						std::cout << "\t\t\tComputing minimal bundles out of " << bases.size() << " bases" << std::endl;
					#endif

					auto minimalSubsets = MinimalSubsets(bses, [&baseCost, &optCost](std::unordered_set<Semiring::FreeMonoid<0>, StreamHash<Semiring::FreeMonoid<0>>> bundle){
						CostType bagCost = CostType::Zero();
						for (auto it = bundle.begin(); it != bundle.end(); it++)
						{
							bagCost = bagCost + baseCost[(*it).at(0) - 1];
						}

						return (bagCost == optCost);
					}, seeds);

					#ifdef VERBOSE
						std::cout << "\t\t\t" << minimalSubsets.size() << " minimal bundles found." << std::endl;
					#endif

					for (auto i = minimalSubsets.begin(); i < minimalSubsets.end(); i++)
					{
						Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>> bundle;
						
						for (auto j = i->begin(); j != i->end(); j++)
						{
							auto p = bases[j->at(0) - 1];
							p.setLabel(dispL);
							bundle.insert(p);
						}

						availPaths.insert(bundle);
					}


					pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(availPaths);

					std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> bund;
					for (auto itr = availPaths.begin(); itr != availPaths.end(); itr++)
					{
						bund = Semiring::Union(bund, itr->getSet());
					}

					pBundle(t,d) = bund;
				}
			}

			#ifdef VERBOSE
				std::cout << "\tStep " << i << std::endl;
				std::cout << pathMatrix << std::endl;
			#endif

			if (i == max_steps)
			{
				return pathMatrix;
			}
			
			

			// Take a step
			#ifdef VERBOSE
				std::cout << "\tCost matrix multiplication step" << std::endl;
			#endif
			auto nC = cMatrix * costStep;
			if (nC == cMatrix)
			{
				return pathMatrix;
			}

			#ifdef VERBOSE
				std::cout << "\tPath matrix multiplication step" << std::endl;
			#endif
			pBundle = pBundle * pathStep;

			/*
			#ifdef VERBOSE
				std::cout << "\tReducing to prior entries in path matrix for unchanged cost" << std::endl;
			#endif
			for (int t = 0; t <N; t++)
			{
				for (int d = 0; d < N; d++)
				{
					if (nC(t,d) == cMatrix(t,d))
					{
						nP(t,d) = pathMatrix(t,d);
					}
				}
			}
			*/

			pCMatrix = cMatrix;
			cMatrix = nC;
		}

		return pathMatrix;
	}

	template<unsigned int N>
	std::vector<double> IndegreeOptimalCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> centralities(N);

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				auto minimals = optimalPaths(i,j).getSet();
				for (auto it = minimals.begin(); it != minimals.end(); it++)
				{
					std::vector<bool> represented(N);
					for (int k = 0; k < N; k++)
					{
						represented[k] = false;
					}
					auto pathSet = (*it).getSet();
					for (auto p = pathSet.begin(); p != pathSet.end(); p++)
					{
						for (int step = 0; step < p->size(); step++)
						{
							int from = (p->at(step) - 1)/N;
							int to = (p->at(step) - 1)%N;
							if (from != to)
								represented[to] = true;
						}
					}

					for (int k = 0; k < N; k++)
					{
						if (represented[k])
						{
							centralities[k] += ((double)1.0 / (double)minimals.size())/(double)(N * N);
						}
					}
				}
			}
		}

		return centralities;
	}

	template<unsigned int N>
	std::vector<double> OutdegreeOptimalCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> centralities(N);

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				auto minimals = optimalPaths(i,j).getSet();
				for (auto it = minimals.begin(); it != minimals.end(); it++)
				{
					std::vector<bool> represented(N);
					for (int k = 0; k < N; k++)
					{
						represented[k] = false;
					}
					auto pathSet = (*it).getSet();
					for (auto p = pathSet.begin(); p != pathSet.end(); p++)
					{
						for (int step = 0; step < p->size(); step++)
						{
							int from = (p->at(step) - 1)/N;
							int to = (p->at(step) - 1)%N;
							if (from != to)
								represented[from] = true;
						}
					}

					for (int k = 0; k < N; k++)
					{
						if (represented[k])
						{
							centralities[k] += ((double)1.0 / (double)minimals.size())/(double)(N * N);
						}
					}
				}
			}
		}

		return centralities;
	}

	template<unsigned int N>
	std::vector<double> StorageCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> centralities(N);

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				auto minimals = optimalPaths(i,j).getSet();
				for (auto it = minimals.begin(); it != minimals.end(); it++)
				{
					std::vector<bool> represented(N);
					for (int k = 0; k < N; k++)
					{
						represented[k] = false;
					}
					auto pathSet = (*it).getSet();
					for (auto p = pathSet.begin(); p != pathSet.end(); p++)
					{
						for (int step = 0; step < p->size(); step++)
						{
							int from = (p->at(step) - 1)/N;
							int to = (p->at(step) - 1)%N;
							if (to == from)
								represented[from] = true;
						}
					}

					for (int k = 0; k < N; k++)
					{
						if (represented[k])
						{
							centralities[k] += ((double)1.0 / (double)minimals.size())/(double)(N * N);
						}
					}
				}
			}
		}

		return centralities;
	}

	template<unsigned int N>
	std::vector<double> OptimalCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> in = IndegreeOptimalCentrality(optimalPaths);
		std::vector<double> out = OutdegreeOptimalCentrality(optimalPaths);
		std::vector<double> store = StorageCentrality(optimalPaths);
		std::vector<double> centralities(N);

		for (int i = 0; i < N; i++)
		{
			centralities[i] = (in[i] + out[i] + store[i])/3.0;
		}

		return centralities;
	}
}



#endif
