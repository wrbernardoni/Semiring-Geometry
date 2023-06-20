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
				std::unordered_set<Semiring::FreeMonoid<N * N>> aPaths;
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

		auto pPMatrix = pathMatrix;

		for (int i = 1; i <= max_steps; i++)
		{
			// Reduce paths
			#ifdef VERBOSE
				std::cout << "\tReducing path possibilities to minimal bundles" << std::endl;
			#endif
			int longestPath = 0;
			for (int t = 0; t < N; t++)
			{
				for (int d = 0; d < N; d++)
				{
					// If we want *all* *all* minimal paths we need to delete this
					// if ((cMatrix(t,d) == pCMatrix(t,d)) && (i > 1))
					// {
					// 	#ifdef VERBOSE
					// 		std::cout << "\t\t Entry ("<< t << "," << d << ") unchanged" << std::endl;
					// 	#endif
					// 	continue;
					// }
					
					if (cMatrix(t,d) == CostType::Zero())
					{
						// #ifdef VERBOSE
						// 	std::cout << "\t\t\tNo available path" << std::endl;
						// #endif
						pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>::Zero();
						continue;
					}

					#ifdef VERBOSE
						std::cout << "\t\t["<< i << "] Reducing entry ("<< t << "," << d << ")" << std::endl;
						std::cout << "\t\t\t Cost: " << std::endl << cMatrix(t,d) << std::endl;
					#endif
					std::unordered_set<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>> availPaths;
					std::unordered_map<Semiring::FreeMonoid<N * N>, int> pMap;

					std::unordered_set<Semiring::FreeMonoid<N * N>> aPaths = pBundle(t,d).getSet();;

					std::vector<Semiring::FreeMonoid<N * N>> bases;
					std::vector<CostType> baseCost;
					std::unordered_set<Semiring::FreeMonoid<0>> bses;

					auto baseCandidates = bases;
					auto bCCandidates = baseCost;

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
							baseCandidates.push_back((*itr));
							bCCandidates.push_back(pathCost);
							// bases.push_back((*itr));
							// baseCost.push_back(pathCost);
							// bses.insert(Semiring::FreeMonoid<0>(bases.size()));
							// pMap[(*itr)] = bases.size();
						}
					}

					// Reduce the base candidates to their minimal elements
					// In cases of two bases with the same cost, keep the ones with shorter path length
					for (int i = 0; i < baseCandidates.size(); i++)
					{
						bool good = true;
						for (int j = 0; j < baseCandidates.size(); j++)
						{
							if (j == i)
								continue;

							if ((bCCandidates[i] >= bCCandidates[j]) && (bCCandidates[i] != bCCandidates[j]) &&  (baseCandidates[i].size() > baseCandidates[j].size()))
							{
								good = false;
								break;
							}
							else if (bCCandidates[i] == bCCandidates[j])
							{
								if (baseCandidates[i].size() > baseCandidates[j].size())
								{
									good = false;
									break;
								}
							}
						}

						if (good)
						{
							bases.push_back(baseCandidates[i]);
							baseCost.push_back(bCCandidates[i]);
							bses.insert(Semiring::FreeMonoid<0>(bases.size()));
							pMap[baseCandidates[i]] = bases.size();
						}
					}
					std::vector<std::unordered_set<Semiring::FreeMonoid<0>>> seeds;

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

					auto optCost = cMatrix(t,d);

					#ifdef VERBOSE
						std::cout << "\t\t\tComputing minimal bundles out of " << bases.size() << " bases" << std::endl;
					#endif

					auto minimalSubsets = MinimalSubsets(bses, [&baseCost, &optCost](std::unordered_set<Semiring::FreeMonoid<0>> bundle){
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
							if (p.size() > longestPath)
							{
								longestPath = p.size();
							}
							bundle.insert(p);
						}

						availPaths.insert(bundle);
					}


					pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(availPaths);

					std::unordered_set<Semiring::FreeMonoid<N * N>> bund;
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
			
			if (longestPath < (i-1))//if (nC == cMatrix)
			{
				return pathMatrix;
			}
			

			// Take a step
			#ifdef VERBOSE
				std::cout << "\tCost matrix multiplication step" << std::endl;
			#endif
			auto nC = cMatrix * costStep;
			// Cut early if paths dont change (faster if we just do costs dont change)

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
			pPMatrix = pathMatrix;
		}

		return pathMatrix;
	}

	template<unsigned int N>
	std::vector<double> IndegreeOptimalCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> centralities(N);
		double nC = 0;

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				auto minimals = optimalPaths(i,j).getSet();
				if (minimals.size() != 0)
				{
					nC += 1;
				}

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
							centralities[k] += ((double)1.0 / (double)minimals.size());
						}
					}
				}
			}
		}

		for (int k = 0; k < N; k++)
		{
			if (nC != 0)
			{
				centralities[k] = centralities[k] / nC;
			}
			else
			{
				centralities[k] = 0;
			}
		}

		return centralities;
	}

	template<unsigned int N>
	std::vector<double> OutdegreeOptimalCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> centralities(N);

		double nC = 0.0;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				auto minimals = optimalPaths(i,j).getSet();
				if (minimals.size() != 0)
				{
					nC += 1.0;
				}

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
							centralities[k] += ((double)1.0 / (double)minimals.size());
						}
					}
				}
			}
		}

		for (int k = 0; k < N; k++)
		{
			if (nC != 0)
			{
				centralities[k] = centralities[k] / nC;
			}
			else
			{
				centralities[k] = 0;
			}
		}

		return centralities;
	}

	template<unsigned int N>
	double Connectivity(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		double nC = 0.0;

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (i == j)
					continue;
				auto minimals = optimalPaths(i,j).getSet();
				if (minimals.size() != 0)
				{
					nC += 1.0;
				}
			}
		}

		return nC / ((double)(N * N - N));
	}

	template<unsigned int N>
	std::vector<double> StorageCentrality(Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> optimalPaths)
	{
		std::vector<double> centralities(N);

		double nC = 0.0;

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				auto minimals = optimalPaths(i,j).getSet();
				if (minimals.size() != 0)
				{
					nC += 1.0;
				}
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
							centralities[k] += ((double)1.0 / (double)minimals.size());
						}
					}
				}
			}
		}

		for (int k = 0; k < N; k++)
		{
			if (nC != 0)
			{
				centralities[k] = centralities[k] / nC;
			}
			else
			{
				centralities[k] = 0;
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
