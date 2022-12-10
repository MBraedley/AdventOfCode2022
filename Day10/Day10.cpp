#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::vector<std::int32_t> adjustRegX{ 1 };

	std::string line;
	while (std::getline(inStrm, line))
	{
		adjustRegX.push_back(0);
		if (line != "noop")
		{
			adjustRegX.push_back(std::stoi(line.substr(5)));
		}
	}

	std::vector<std::int32_t> regX;
	regX.reserve(adjustRegX.size());
	std::partial_sum(adjustRegX.begin(), adjustRegX.end(), std::back_inserter(regX));

	std::cout << regX[19] * 20 + regX[59] * 60 + regX[99] * 100 + regX[139] * 140 + regX[179] * 180 + regX[219] * 220 << "\n";

	for (std::size_t i = 0; i < regX.size(); i++)
	{
		std::int32_t xPos = i % 40;
		if (std::abs(regX[i] - xPos) <= 1)
		{
			std::cout << "#";
		}
		else
		{
			std::cout << " ";
		}

		if (xPos == 39)
		{
			std::cout << "\n";
		}
	}
}
