#ifndef SET_UTILITIES_H_
#define SET_UTILITIES_H_

#include <utility>
#include <unordered_set>
#include <functional>
#include <queue>
#include <stdlib.h>

#include "UtilitiesDefinitions.h"

#include <iostream>
#include <random>
#include <sstream>

namespace Semiring
{
	template<typename T> class StreamHash {
	public:
	    size_t operator()(const T& p) const
	    {
	    	std::stringstream x;
	    	x << p;
	        return std::hash<std::string>()(x.str());
	    }
	};

	template<typename S, typename T>
	std::unordered_set<T> Apply(std::unordered_set<S> set, std::function<T(S)> f)
	{
		std::unordered_set<T> ret;
		for (auto itr = set.begin(); itr != set.end(); itr++)
		{
			ret.insert(f(*itr));
		}

		return ret;
	};

	template<typename S> Subset(std::unordered_set<S> set, std::function<bool(S)> f)
	{
		std::unordered_set<S> ret;
		for (auto itr = set.begin(); itr != set.end(); itr++)
		{
			if (f(*itr))
				ret.insert(*itr);
		}
		return ret;
	};

	template<typename S> 
	std::unordered_set<S> Union(std::unordered_set<S> setA, std::unordered_set<S> setB)
	{
		std::unordered_set<S> set;

		if (setA.size() > setB.size())
		{
			set = setA;
			for (auto itr = setB.begin(); itr != setB.end(); itr++)
			{
				set.insert(*itr);
			}
		}
		else
		{
			set = setB;
			for (auto itr = setA.begin(); itr != setA.end(); itr++)
			{
				set.insert(*itr);
			}
		}	

		return set;
	};

	template<typename S>
	std::unordered_set<S> Intersect(std::unordered_set<S> left, std::unordered_set<S> right)
	{
		std::unordered_set<S> intersect;
		std::unordered_set<S>* small;
		std::unordered_set<S>* big;

		if (left.size() <= right.size())
		{
			small = &left;
			big = &right;
		}
		else
		{
			small = &right;
			big = &left;
		}

		for (auto it = small->begin(); it != small->end(); it++)
		{
			if (big->count(*it) != 0)
			{
				intersect.insert((*it));
			}
		}

		return intersect;
	}

	template<typename S>
	bool SubsetEq(std::unordered_set<S> left, std::unordered_set<S> right)
	{
		if (left.size() > right.size())
			return false;

		for (auto it = left.begin(); it != left.end(); it++)
		{
			if (right.count(*it) == 0)
			{
				return false;
			}
		}

		return true;
	}

	template<typename S>
	bool SetEq(std::unordered_set<S> left, std::unordered_set<S> right)
	{
		if (left.size() != right.size())
			return false;
		return SubsetEq(left, right);
	}

	template<typename S>
	void PrintSet(std::unordered_set<S> s)
	{
		std::cout << "{";
		bool empty = true;;
		for (auto it = s.begin(); it != s.end(); it++)
		{
			if (!empty)
			{
				std::cout << ", ";
			}
			else
			{
				empty = false;
			}
			std::cout << (*it);
		}
		std::cout << "}";
	}

	// Generates an arbitrary subset, aiming to be around half the size of the set.
	// Assumes that the number of lower bound restrictions is higher than upper bound restrictions
	template<typename S>
	std::unordered_set<S> GenerateSubset(std::unordered_set<S> baseSet, std::vector<std::unordered_set<S>> notSuperset, std::vector<std::unordered_set<S>> notSubset, int goalSize)
	{
		if (baseSet.size() <= 1)
			return std::unordered_set<S>();

		std::vector<S> ele(baseSet.begin(), baseSet.end());
		std::random_shuffle(ele.begin(), ele.end());
		int cBestSize = -1;
		std::unordered_set<S> currentBest;

		std::deque<std::unordered_set<S>> q;

		std::vector<std::unordered_set<S>> notSup;
		for (int i = 0; i < notSuperset.size(); i++)
		{
			if (SubsetEq(notSuperset[i], baseSet))
			{
				notSup.push_back(notSuperset[i]);
			}
		}

		std::vector<std::unordered_set<S>> notSub;
		for (int i = 0; i < notSubset.size(); i++)
		{
			std::unordered_set<S> s = Intersect(notSubset[i], baseSet);
			bool add = true;
			for (int j = 0; j < notSub.size(); j++)
			{
				if (SetEq(notSub[j], s))
				{
					add = false;
					break;
				}
			}

			if (add)
			{
				notSub.push_back(s);
			}
		}

		std::deque<int> lIns;
		std::deque<std::unordered_set<S>> setQ;

		bool breadth = false;

		for (int i = 0; i < ele.size(); i++)
		{
			std::unordered_set<S> testS = baseSet;
			testS.erase(ele[i]);
			lIns.push_back(i);
			setQ.push_back(testS);
		}

		while(setQ.size() != 0)
		{
			std::unordered_set<S> topS = setQ.front();
			int lAdd = lIns.front();
			setQ.pop_front();
			lIns.pop_front();

			if (topS.size() == 0)
				continue;

			// Check if top includes anything in notSuperset
			bool bad = false;
			for (auto it = notSub.begin(); it != notSub.end(); it++)
			{
				if (SubsetEq(topS, (*it)))
				{
					bad = true;
					break;
				}
			}

			if (!bad)
			{
				std::unordered_set<S> testS;
				for (int i = 0; i < lAdd; i++)
				{
					if (topS.count(ele[i]) != 0)
						testS.insert(ele[i]);
				}

				for (auto it = notSup.begin(); it != notSup.end(); it++)
				{
					if (SubsetEq((*it), topS))
					{
						bad = true;
						break;
					}
				}
			}

			if (bad)
			{
				if (cBestSize != -1)
				{
					breadth = true;
				}
				continue;
			}

			// Check if closer to optimal than current best
				// If so check if it is a subset of dependents
			if ((abs((int) topS.size() - goalSize) < abs(cBestSize - goalSize)) || (cBestSize == -1))
			{
				for (auto it = notSup.begin(); it != notSup.end(); it++)
				{
					if (SubsetEq((*it), topS))
					{
						bad = true;
						break;
					}
				}

				if (!bad)
				{
					cBestSize = topS.size();
					currentBest = topS;

					if (cBestSize == goalSize)
					{
						break;
					}
				}
			}
			else if ((cBestSize != -1) && topS.size() < goalSize)
			{
				continue;
			}

			if (breadth)
			{
				for (int i = lAdd + 1; i < ele.size(); i++)
				{
					std::unordered_set<S> newS;
					newS = topS;
					newS.erase(ele[i]);
					setQ.push_back(newS);
					lIns.push_back(i);
				}
			}
			else
			{
				for (int i = ele.size() - 1; i >= lAdd + 1; i--)
				{
					std::unordered_set<S> newS;
					newS = topS;
					newS.erase(ele[i]);
					setQ.push_front(newS);
					lIns.push_front(i);
				}
			}
		}

		return currentBest;
	}

	// Find minimal subsets while minimizing calls to oracle
	// Assumes that oracle(all) = true and that oracle({}) = false
	// Also assumes that if X <= Y then oracle(X) = true implies oracle(Y) = true
	template<typename S, typename Fun>
	std::vector<std::unordered_set<S>> MinimalSubsets(std::unordered_set<S> all, Fun oracle, std::vector<std::unordered_set<S>> initialMinimals = std::vector<std::unordered_set<S>>())
	{
		std::vector<std::unordered_set<S>> minimals;
		std::vector<std::unordered_set<S>> dependents;
		dependents.push_back(std::unordered_set<S>());

		auto cmp = [](std::unordered_set<S> left, std::unordered_set<S> right) {return left.size() > right.size();};
		std::priority_queue<std::unordered_set<S>, std::vector<std::unordered_set<S>>, decltype(cmp)> que;
		
		std::unordered_set<S> top = all;
		que.push(all);

		for (int i = 0; i < initialMinimals.size(); i++)
		{
			que.push(initialMinimals[i]);
		}

		#ifdef VERBOSE
			unsigned int minCount = 0;
			std::cout << "\t\t\tFinding minimal subsets out of set of size " << all.size() << std::endl;
			unsigned int oracleCalls = 0;
		#endif

		while (que.size() != 0)
		{
			std::unordered_set<S> top = que.top();

			// See if there is a subset of top, not containing any minimals and not being contained in any dependents
			//		Test that subset, if dependent add it to dependent list
			//				if independent add it to queue
			int depC = 0;
			int depN = 0;
			for (int i = 0; i < dependents.size();i++)
			{
				int subE = Intersect(dependents[i], top).size();
				//if (subE != 0)
				//{
					depC += subE;
					depN++;
				//}
			}
			int avgDepsize = 0;
			if (depN != 0)
				avgDepsize = (depC + depN - 1)/(depN);

			int goalSize = (top.size() + avgDepsize)/2;
			if (minimals.size() > 10)
				goalSize = (top.size() + std::max((int)((minCount + minimals.size() - 1)/minimals.size()), avgDepsize))/2;

			if (goalSize > top.size() - 1)
				goalSize = top.size() - 1;//top.size()/2 + top.size()%2;
			#ifdef VERBOSE
				std::cout << "\t\t\t\t(g)" << top.size() << "-> ? #min:" << minimals.size() << "(" << (minimals.size() != 0 ? ((minCount + minimals.size() - 1)/minimals.size())  : 0) << ") #dep:" << dependents.size() << "(" << avgDepsize << ") #queue:" << que.size() << " goalSize:" << goalSize << " oracleCalls:" << oracleCalls;
				std::cout << "      \r" << std::flush;
			#endif
			std::unordered_set<S> subset = GenerateSubset(top, minimals, dependents, goalSize);
			#ifdef VERBOSE
				std::cout << "\t\t\t\t(c)" << top.size() << "->" << subset.size() << " #min:" << minimals.size() << "(" << (minimals.size() != 0 ? ((minCount + minimals.size() - 1)/minimals.size())  : 0) << ") #dep:" << dependents.size() << "(" << avgDepsize <<") #queue:" << que.size() << " goalSize:" << goalSize << " oracleCalls:" << oracleCalls;
				std::cout << "      \r" << std::flush;
			#endif
			if (subset.size() != 0)
			{
				#ifdef VERBOSE
					oracleCalls++;
				#endif
				if(oracle(subset))
				{
					que.push(subset);
				}
				else
				{
					std::erase_if(dependents, [&subset](auto const& e){ return SubsetEq(e, subset);});
					dependents.push_back(subset);
				}

				continue;
			}
			//		If there are no such subsets, add top to minimal if it does not contain any existing minimals
			que.pop();
			bool bad = false;
			for (auto it = minimals.begin(); it != minimals.end(); it++)
			{
				if (SubsetEq(*it, top))
				{
					bad = true;
					break;
				}
			}
			if (!bad)
			{
				#ifdef VERBOSE
					minCount += top.size();
				#endif
				minimals.push_back(top);

				// Just finds one minimal
				// break;
			}
		}

		#ifdef VERBOSE
			std::cout << "\n";
		#endif

		return minimals;
	};
}

#endif
