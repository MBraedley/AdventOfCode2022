#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <set>
#include <regex>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::set<std::pair<std::int32_t, std::int32_t>> blocks;

	std::regex rockRanges("(\\d+),(\\d+)");
	std::smatch m;

	std::int32_t maxDepth = 0;

	std::string line;
	while (std::getline(inStrm, line))
	{
		std::vector<std::pair<std::int32_t, std::int32_t>> rocks;
		while (std::regex_search(line, m, rockRanges))
		{
			rocks.emplace_back(std::stoi(m[1]), std::stoi(m[2]));
			line = m.suffix();
		}

		for (std::size_t i{ 1 }; i < rocks.size(); i++)
		{
			if (rocks[i - 1].first == rocks[i].first)
			{
				for (int y = std::min(rocks[i - 1].second, rocks[i].second); y <= std::max(rocks[i - 1].second, rocks[i].second); y++)
				{
					blocks.emplace(rocks[i].first, y);
				}
			}
			else
			{
				for (int x = std::min(rocks[i - 1].first, rocks[i].first); x <= std::max(rocks[i - 1].first, rocks[i].first); x++)
				{
					blocks.emplace(x, rocks[i].second);
				}
			}

			if (std::max(rocks[i - 1].second, rocks[i].second) > maxDepth)
			{
				maxDepth = std::max(rocks[i - 1].second, rocks[i].second);
			}
		}
	}

	std::size_t startingBlockCount = blocks.size();

	std::uint16_t part = 1;
	std::function<bool(std::int32_t, std::int32_t)> checkSand = [&](std::int32_t x, std::int32_t y) -> bool
	{
		y;
		if (part == 1 && y > maxDepth)
		{
			return false;
		}
		else if (part == 2 && y == 0 && blocks.contains({x, y}))
		{
			return false;
		}
		else if (part == 2 && y + 1 == maxDepth + 2)
		{
			auto [_, inserted] = blocks.emplace(x, y);
			if (!inserted)
			{
				throw std::exception("sand not added to pile");
			}
			return true;
		}

		if (!blocks.contains({x, y + 1}))
		{
			return checkSand(x, y + 1);
		}
		else if (!blocks.contains({x - 1, y + 1}))
		{
			return checkSand(x - 1, y + 1);
		}
		else if (!blocks.contains({ x + 1, y + 1 }))
		{
			return checkSand(x + 1, y + 1);
		}
		else
		{
			auto [_, inserted] = blocks.emplace(x, y);
			if (!inserted)
			{
				throw std::exception("sand not added to pile");
			}
			return true;
		}
	};

	while (checkSand(500, 0))
	{
	}

	std::cout << blocks.size() - startingBlockCount << "\n";

	//Part 2
	part = 2;

	while (checkSand(500, 0))
	{
	}

	std::cout << blocks.size() - startingBlockCount << "\n";
}
