#ifndef SHORTEST_PATH_H_
#define SHORTEST_PATH_H_

#include <Semirings.h>

#include <iostream>

#include <vector>

#define VERBOSE 1

namespace Semiring
{
	std::vector<std::vector<int>> ChoiceBuilder(int N, int C, int minE, Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>> built, std::vector<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>>>& constraints)
	{
		for (int l = 0; l < constraints.size(); l++)
		{
			if (built <= constraints[l])
			{
				return std::vector<std::vector<int>>();
			}
		}

		if (C > 1)
		{
			// if (C > (N - minE))
			// 	return std::vector<std::vector<int>>();

			std::vector<std::vector<int>> choices;

			for (int  i = minE; i < N; i++)
			{
				auto subC = ChoiceBuilder(N, C - 1, i+1, built + Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>>(Semiring::FreeMonoid<0>(i + 1)), constraints);
				for (int j = 0; j < subC.size(); j++)
				{
					std::vector<int> c;
					c.push_back(i);
					c.insert(c.end(), subC[j].begin(), subC[j].end());
					choices.push_back(c);
				}
			}

			return choices;
		}

		if (C == 1)
		{
			std::vector<std::vector<int>> choices;
			for (int i = minE; i < N; i++)
			{
				bool redundant = false;
				for (int l = 0; l < constraints.size(); l++)
				{
					if ((built + Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>>(Semiring::FreeMonoid<0>(i + 1))) <= constraints[l])
					{
						redundant = true;
						break;
					}
				}
				if (!redundant)
				{
					std::vector<int> c;
					c.push_back(i);
					choices.push_back(c);
				}
			}
			return choices;
		}

		return std::vector<std::vector<int>>();
		
	}

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
		Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> pathMatrix([&costMatrix](unsigned int t, unsigned int d)->Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>> {
			if (costMatrix(t,d) != CostType::Zero())
				return Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>(Semiring::FreeMonoid<N * N>(t * N + d + 1)));
			else
			{
				return Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>());
			}
		});

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

		#ifdef VERBOSE
			std::cout << "\tSetting up step matrices" << std::endl;
		#endif
		Semiring::Matrix<CostType,N,N> costStep = costMatrix + Semiring::Matrix<CostType,N,N>::One();
		pathMatrix =  pathMatrix + Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N>::One();
		auto pathStep = pathMatrix;
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
					if ((cMatrix(t,d) == pCMatrix(t,d)) && (i > 1))
						continue;

					#ifdef VERBOSE
						std::cout << "\t\t Reducing entry ("<< t << "," << d << ")" << std::endl;
					#endif
					if (cMatrix(t,d) == CostType::Zero())
					{
						#ifdef VERBOSE
							std::cout << "\t\t\tNo available path" << std::endl;
						#endif
						pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>::Zero();
						continue;
					}
					std::unordered_set<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>, StreamHash<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>> availPaths;

					auto opSet = pathMatrix(t,d).getSet();
					std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> aPaths;
					for (auto itr = opSet.begin(); itr != opSet.end(); itr++)
					{
						aPaths = Semiring::Union(aPaths, itr->getSet());
					}

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
						}
					}

					auto optCost = cMatrix(t,d);

					auto minimalSubsets = MinimalSubsets(bses, [&baseCost, &optCost](std::unordered_set<Semiring::FreeMonoid<0>, StreamHash<Semiring::FreeMonoid<0>>> bundle){
						CostType bagCost = CostType::Zero();
						for (auto it = bundle.begin(); it != bundle.end(); it++)
						{
							bagCost = bagCost + baseCost[(*it).at(0) - 1];
						}

						return (bagCost == optCost);
					});

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

					/*
					std::vector<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>>> usedBases;

					for (int C = 1; C <= bases.size(); C++)
					{
						// Shortcutting to only find collections of minimal rank, remove this to find all
						if (usedBases.size() != 0)
							break;

						// Number of choices
						auto choices = ChoiceBuilder(bases.size(), C,0, Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>>(), usedBases);
						#ifdef VERBOSE
							std::cout << "\t\t\tComputing "<< C << " element choices (" << choices.size() << ") of " << bases.size() << " bases\r" << std::flush;
						#endif

						for (int c = 0; c < choices.size(); c++)
						{
							#ifdef VERBOSE
								std::cout << "\t\t\tComputing "<< C << " element choices (" << c + 1 << "/" << choices.size() << ")[" << availPaths.size() <<"] of " << bases.size() << " bases\r" << std::flush;
							#endif
							Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>> bU;

							// Check path attains cost
							CostType bagCost = CostType::Zero();
							std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> choice;
							for (int k = 0; k < choices[c].size(); k++)
							{
								bU.insert(choices[c][k] + 1);
								auto p = bases[choices[c][k]];
								auto pCost = baseCost[choices[c][k]];
								p.setLabel(dispL);
								if (choice.count(p) == 0)
								{
									choice.insert(p);
									bagCost = bagCost + pCost;
								}	
							}

							if (bagCost == cMatrix(t,d))
							{
								availPaths.insert(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>(choice));
								usedBases.push_back(bU);
							}

							#ifdef VERBOSE
								std::cout << "\t\t\tComputing "<< C << " element choices (" << c + 1 << "/" << choices.size() << ")[" << availPaths.size() <<"] of " << bases.size() << " bases\r" << std::flush;
							#endif
						}
					}
					#ifdef VERBOSE
						if (bases.size() > 0)
							std::cout << std::endl;
					#endif
					*/


					pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(availPaths);
				}
			}

			#ifdef VERBOSE
				std::cout << "\tStep " << i << std::endl;
				std::cout << pathMatrix << std::endl;
			#endif
			
			

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
			auto nP = pathMatrix * pathStep;

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
			pCMatrix = cMatrix;
			cMatrix = nC;
			pathMatrix = nP;
		}

		return pathMatrix;
	}
}

#endif
