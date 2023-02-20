#ifndef SET_UTILITIES_H_
#define SET_UTILITIES_H_

#include <utility>
#include <unordered_set>
#include <functional>

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
	}
}

#endif
