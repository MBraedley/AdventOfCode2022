#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stack>
#include <functional>
#include <regex>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::string> instructions;
	std::vector<std::stack<char>> piles1;
	std::vector<std::stack<char>> piles2;
	std::ifstream inStrm(inputFile);

	std::string line;
	std::uint64_t calCount = 0;

	std::stack<std::string> initialPiles;
	while (std::getline(inStrm, line) && !line.empty())
	{
		initialPiles.push(line);
	}

	while (std::getline(inStrm, line))
	{
		instructions.push_back(line);
	}

	std::size_t pileCount = (initialPiles.top().size() + 1) / 4;
	piles1.resize(pileCount);
	piles2.resize(pileCount);
	initialPiles.pop();
	while (!initialPiles.empty())
	{
		const auto& layer = initialPiles.top();
		for (std::size_t i = 0; i < pileCount; i++)
		{
			if (char c = layer[i * 4 + 1]; c != ' ')
			{
				piles1[i].push(c);
				piles2[i].push(c);
			}
		}
		initialPiles.pop();
	}

	std::regex move("move (\\d+) from (\\d+) to (\\d+)");
	std::smatch m;

	for (const auto& inst : instructions)
	{
		std::regex_match(inst, m, move);

		std::size_t count = std::stoull(m[1]);
		std::size_t from = std::stoull(m[2]) - 1;
		std::size_t to = std::stoull(m[3]) - 1;

		for (std::size_t i = 0; i < count; i++)
		{
			piles1[to].push(piles1[from].top());
			piles1[from].pop();
		}
	}

	for (const auto& p : piles1)
	{
		std::cout << p.top();
	}
	std::cout << "\n";

	//Part 2

	for (const auto& inst : instructions)
	{
		std::regex_match(inst, m, move);

		std::size_t count = std::stoull(m[1]);
		std::size_t from = std::stoull(m[2]) - 1;
		std::size_t to = std::stoull(m[3]) - 1;

		std::stack<char> temp;
		for (std::size_t i = 0; i < count; i++)
		{
			temp.push(piles2[from].top());
			piles2[from].pop();
		}

		while (!temp.empty())
		{
			piles2[to].push(temp.top());
			temp.pop();
		}
	}

	for (const auto& p : piles2)
	{
		std::cout << p.top();
	}
	std::cout << "\n";
}
