#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::uint64_t> snacks;
	std::ifstream inStrm(inputFile);

	std::string line;
	std::uint64_t calCount = 0;
	while (std::getline(inStrm, line))
	{
		if (!line.empty())
		{
			calCount += std::stoull(line);
		}
		else
		{
			snacks.push_back(calCount);
			calCount = 0;
		}
	}

	std::cout << *std::max_element(snacks.begin(), snacks.end()) << "\n";

	//Part 2
	std::sort(snacks.begin(), snacks.end(), std::greater<std::uint64_t>());
	std::cout << snacks[0] + snacks[1] + snacks[2] << "\n";
}
