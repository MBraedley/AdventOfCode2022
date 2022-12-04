#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <regex>

struct Assignment
{
	std::uint32_t low;
	std::uint32_t high;
};

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::pair<Assignment, Assignment>> assignments;
	std::ifstream inStrm(inputFile);

	std::string line;
	std::regex parseInput("(\\d+)-(\\d+),(\\d+)-(\\d+)");
	
	std::uint64_t encloseCount = 0;
	std::uint64_t overlapCount = 0;
	
	while (std::getline(inStrm, line))
	{
		std::smatch m;
		std::regex_match(line, m, parseInput);
		Assignment elf1{ std::stoul(m[1]), std::stoul(m[2]) };
		Assignment elf2{ std::stoul(m[3]), std::stoul(m[4]) };

		assignments.emplace_back(elf1, elf2);

		if ((elf1.low <= elf2.low && elf1.high >= elf2.high)
			|| (elf1.low >= elf2.low && elf1.high <= elf2.high))
		{
			encloseCount++;
		}

		//Part 2
		if (!(elf1.high < elf2.low || elf1.low > elf2.high))
		{
			overlapCount++;
		}
	}

	std::cout << encloseCount << "\n" << overlapCount << "\n";

}
