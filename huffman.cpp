#include <iostream>
#include <vector>
#include <utility>
#include <queue>
#include <algorithm>
#include <fstream>
#include <list>

struct Symbol
{
	std::list<std::size_t> symbols;
	double f;
	bool operator()(const Symbol& lhs, const Symbol& rhs)
	{	// this function is intended for min-heap data structure
		return lhs.f > rhs.f;
	}
	void increment(std::vector<std::size_t>& length) const
	{	// this function is used to increase the length of symbols contained in this object
		for(auto& sym : symbols)
		{
			++length[sym];
		}
	}
};

std::pair<std::size_t, std::size_t> huffman(const std::vector<Symbol>& f)
{	// computes minimum and maximum lengths of huffman coding
	std::priority_queue<Symbol, std::vector<Symbol>, Symbol> q(f.begin(), f.end());
	std::vector<std::size_t> length(f.size(), 0);
	while(q.size() != 1)
	{
		// get the two with minimum freq or weight
		Symbol a = q.top();		q.pop();
		Symbol b = q.top();		q.pop();
		// combine them into single one and push
		Symbol ab;
		ab.f = a.f + b.f;
		// the following works in O(1) time due the nature of std::list
		ab.symbols.splice(ab.symbols.end(), a.symbols);
		ab.symbols.splice(ab.symbols.end(), b.symbols);
		// increase length of each symbol by one
		ab.increment(length);
		q.push(ab);
	}
	// return min and max of lengths
	auto [min, max] = std::minmax_element(length.begin(), length.end());
	return {*min, *max};
}


int main()
{
	// read file 
	std::ifstream file("huffman.txt");
	std::size_t n = 0;
	double w = 0.0;
	file >> n;
	std::vector<Symbol> s(n);
	for(std::size_t i=0; i<n; ++i)
	{	// in this data set, instead of frequencies, weights are given
		file >> w;
		s[i] = {{i}, w};
	}
	// compute min and max lengths
	auto [min, max] = huffman(s);
	std::cout << "Min=" << min << " Max=" << max << "\n";
	return 0;
}
