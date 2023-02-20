#ifndef SHORTEST_PATH_H_
#define SHORTEST_PATH_H_

#include <Semirings.h>

#include <iostream>

#include <vector>

namespace Semiring
{
	std::vector<std::vector<int>> ChoiceBuilder(int N, int C, int minE = 0)
	{
		if (C > 1)
		{
			// if (C > (N - minE))
			// 	return std::vector<std::vector<int>>();

			std::vector<std::vector<int>> choices;

			for (int  i = minE; i < N; i++)
			{
				auto subC = ChoiceBuilder(N, C - 1, i+1);
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
				std::vector<int> c;
				c.push_back(i);
				choices.push_back(c);
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
	Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N> MinimalPaths(Semiring::Matrix<CostType,N,N> costMatrix, unsigned int max_steps)
	{
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


		Semiring::Matrix<CostType,N,N> costStep = costMatrix + Semiring::Matrix<CostType,N,N>::One();
		pathMatrix =  pathMatrix + Semiring::Matrix<Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>,N,N>::One();
		auto pathStep = pathMatrix;
		auto cMatrix = costStep;

		for (int i = 1; i <= max_steps; i++)
		{
			// Reduce paths
			for (int t = 0; t < N; t++)
			{
				for (int d = 0; d < N; d++)
				{
					std::unordered_set<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>, StreamHash<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>> availPaths;

					auto opSet = pathMatrix(t,d).getSet();
					std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> aPaths;
					for (auto itr = opSet.begin(); itr != opSet.end(); itr++)
					{
						aPaths = Semiring::Union(aPaths, itr->getSet());
					}
					std::vector<Semiring::FreeMonoid<N * N>> bases;
					std::copy(aPaths.begin(), aPaths.end(), std::back_inserter(bases));

					std::vector<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>>> usedBases;

					for (int C = 1; C <= bases.size(); C++)
					{
						// Number of choices
						auto choices = ChoiceBuilder(bases.size(), C);
						for (int c = 0; c < choices.size(); c++)
						{
							Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<0>> bU;
							for (int k = 0; k < choices[c].size(); k++)
							{
								bU.insert(choices[c][k] + 1);
							}
							if (C > 1)
							{
								bool redundant = false;
								for (int l = 0; l < usedBases.size(); l++)
								{
									if (bU <= usedBases[l])
									{
										redundant = true;
										break;
									}
								}
								if (redundant)
									continue;
							}

							// Check for redundancy
							std::unordered_set<Semiring::FreeMonoid<N * N>, StreamHash<Semiring::FreeMonoid<N * N>>> choice;
							for (int k = 0; k < choices[c].size(); k++)
							{
								auto p = bases[choices[c][k]];
								p.setLabel(dispL);
								choice.insert(p);
							}

							// Check path attains cost
							CostType bagCost = CostType::Zero();
							for (auto itr = choice.begin(); itr != choice.end(); itr++)
							{
								CostType pathCost = CostType::One();
								for (int step = 0; step < (*itr).size(); step++)
								{
									if ((*itr).at(step) == 0)
										continue;
									int edge = (*itr).at(step) - 1;
									int r = edge/N;
									int c = edge%N;
									pathCost = pathCost * costMatrix(r,c);
								}
								bagCost = bagCost + pathCost;
							}

							if (bagCost == cMatrix(t,d))
							{
								availPaths.insert(Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>(choice));
								usedBases.push_back(bU);
							}
						}
					}

					pathMatrix(t,d) = Semiring::FreeIdempotentSemiring<Semiring::FreeIdempotentSemiring<Semiring::FreeMonoid<N * N>>>(availPaths);
				}
			}

			std::cout << "Step " << i << std::endl;
			std::cout << pathMatrix << std::endl;
			

			// Take a step
			auto nC = cMatrix * costStep;
			if (nC == cMatrix)
			{
				return pathMatrix;
			}

			auto nP = pathMatrix * pathStep;

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
			cMatrix = nC;
			pathMatrix = nP;
		}

		return pathMatrix;
	}
}

#endif
