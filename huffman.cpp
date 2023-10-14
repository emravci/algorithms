#include <iostream>
#include <vector>
#include <utility>
#include <queue>
#include <algorithm>
#include <fstream>

struct Symbol
{
	std::vector<std::size_t> symbols;
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
{	// computes minimum and maximum length of huffman encoding
	std::priority_queue<Symbol, std::vector<Symbol>, Symbol> q(f.begin(), f.end());
	std::vector<std::size_t> length(f.size(), 0);
	while(q.size() != 1)
	{
		// get the two with minimum freq
		Symbol a = q.top();		q.pop();
		Symbol b = q.top();		q.pop();
		// combine them into single one and push
		Symbol ab;
		ab.f = a.f + b.f;
		ab.symbols.insert(ab.symbols.end(), a.symbols.begin(), a.symbols.end());
		ab.symbols.insert(ab.symbols.end(), b.symbols.begin(), b.symbols.end());
		q.push(ab);
		// increase length of each symbol by one
		a.increment(length);
		b.increment(length);
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
	// compute average length
	auto [min, max] = huffman(s);
	std::cout << "Min=" << min << " Max=" << max << "\n";
	return 0;
}
