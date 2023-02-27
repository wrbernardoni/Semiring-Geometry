#ifndef SET_UTILITIES_H_
#define SET_UTILITIES_H_

#include <utility>
#include <unordered_set>
#include <functional>
#include <queue>
#include <stdlib.h>

#include <iostream>

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
	std::unordered_set<T, StreamHash<T>> Apply(std::unordered_set<S, StreamHash<S>> set, std::function<T(S)> f)
	{
		std::unordered_set<T, StreamHash<T>> ret;
		for (auto itr = set.begin(); itr != set.end(); itr++)
		{
			ret.insert(f(*itr));
		}

		return ret;
	};

	template<typename S> Subset(std::unordered_set<S, StreamHash<S>> set, std::function<bool(S)> f)
	{
		std::unordered_set<S, StreamHash<S>> ret;
		for (auto itr = set.begin(); itr != set.end(); itr++)
		{
			if (f(*itr))
				ret.insert(*itr);
		}
		return ret;
	};

	template<typename S> 
	std::unordered_set<S, StreamHash<S>> Union(std::unordered_set<S, StreamHash<S>> setA, std::unordered_set<S, StreamHash<S>> setB)
	{
		std::unordered_set<S, StreamHash<S>> set;

		for (auto itr = setA.begin(); itr != setA.end(); itr++)
		{
			set.insert(*itr);
		}

		for (auto itr = setB.begin(); itr != setB.end(); itr++)
		{
			set.insert(*itr);
		}

		return set;
	};

	template<typename S>
	bool SubsetEq(std::unordered_set<S, StreamHash<S>> left, std::unordered_set<S, StreamHash<S>> right)
	{
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
	bool SetEq(std::unordered_set<S, StreamHash<S>> left, std::unordered_set<S, StreamHash<S>> right)
	{
		return (SubsetEq(left, right) && SubsetEq(right, left));
	}

	template<typename S>
	void PrintSet(std::unordered_set<S, StreamHash<S>> s)
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
		std::cout << "}" << std::endl;
	}

	// Generates an arbitrary subset, aiming to be around half the size of the set.
	template<typename S>
	std::unordered_set<S,StreamHash<S>> GenerateSubset(std::unordered_set<S,StreamHash<S>> baseSet, std::vector<std::unordered_set<S,StreamHash<S>>> notSuperset, std::vector<std::unordered_set<S,StreamHash<S>>> notSubset)
	{
		if (baseSet.size() <= 1)
			return std::unordered_set<S,StreamHash<S>>();

		std::vector<S> ele(baseSet.begin(), baseSet.end());
		int goalSize = baseSet.size() / 2 + (baseSet.size() % 2);
		int cBestSize = -1;
		std::unordered_set<S,StreamHash<S>> currentBest;

		std::deque<std::unordered_set<S,StreamHash<S>>> q;

		std::deque<int> lIns;
		std::deque<std::unordered_set<S,StreamHash<S>>> setQ;

		for (int i = 0; i < ele.size(); i++)
		{
			std::unordered_set<S,StreamHash<S>> testS;
			testS.insert(ele[i]);
			lIns.push_back(i);
			setQ.push_back(testS);
		}

		while(setQ.size() != 0)
		{
			std::unordered_set<S,StreamHash<S>> topS = setQ.front();
			int lAdd = lIns.front();
			setQ.pop_front();
			lIns.pop_front();

			if (topS.size() == baseSet.size())
				continue;

			// Check if top includes anything in notSuperset
			bool bad = false;
			for (auto it = notSuperset.begin(); it != notSuperset.end(); it++)
			{
				if (SubsetEq((*it),topS))
				{
					bad = true;
					break;
				}
			}
			if (bad)
			{
				continue;
			}

			// Check if closer to optimal than current best
				// If so check if it is a subset of dependents
			if ((abs((int) topS.size() - goalSize) < abs(cBestSize - goalSize)) || (cBestSize == -1))
			{
				for (auto it = notSubset.begin(); it != notSubset.end(); it++)
				{
					if (SubsetEq(topS,(*it)))
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

			for (int i = lAdd + 1; i < ele.size(); i++)
			{
				std::unordered_set<S,StreamHash<S>> newS;
				newS = topS;
				newS.insert(ele[i]);
				setQ.push_front(newS);
				lIns.push_front(i);
			}
		}

		return currentBest;
	}

	// Find minimal subsets while minimizing calls to oracle
	// Assumes that oracle(all) = true and that oracle({}) = false
	// Also assumes that if X <= Y then oracle(X) = true implies oracle(Y) = true
	template<typename S, typename Fun>
	std::vector<std::unordered_set<S,StreamHash<S>>> MinimalSubsets(std::unordered_set<S,StreamHash<S>> all, Fun oracle)
	{
		std::vector<std::unordered_set<S,StreamHash<S>>> minimals;
		std::vector<std::unordered_set<S,StreamHash<S>>> dependents;
		dependents.push_back(std::unordered_set<S,StreamHash<S>>());

		auto cmp = [](std::unordered_set<S,StreamHash<S>> left, std::unordered_set<S,StreamHash<S>> right) {return left.size() > right.size();};
		std::priority_queue<std::unordered_set<S,StreamHash<S>>, std::vector<std::unordered_set<S,StreamHash<S>>>, decltype(cmp)> que;
		
		std::unordered_set<S,StreamHash<S>> top = all;
		que.push(all);

		while (que.size() != 0)
		{
			std::unordered_set<S,StreamHash<S>> top = que.top();

			// See if there is a subset of top, not containing any minimals and not being contained in any dependents
			//		Test that subset, if dependent add it to dependent list
			//				if independent add it to queue
			std::unordered_set<S,StreamHash<S>> subset = GenerateSubset(top, minimals, dependents);
			if (subset.size() != 0)
			{
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
				minimals.push_back(top);
		}

		return minimals;
	};
}

#endif
