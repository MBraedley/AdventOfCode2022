#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <deque>

#define NOMINMAX

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::vector<std::string> areaMap;
	std::pair<std::size_t, std::size_t> start;
	std::pair<std::size_t, std::size_t> end;

	std::string line;
	while (std::getline(inStrm, line))
	{
		if (auto xPos = line.find_first_of('S'); xPos != line.npos)
		{
			start = std::make_pair(xPos, areaMap.size());
			line[xPos] = 'a';
		}
		if (auto xPos = line.find_first_of('E'); xPos != line.npos)
		{
			end = std::make_pair(xPos, areaMap.size());
			line[xPos] = 'z';
		}
		areaMap.push_back(line);
	}

	std::map<std::pair<std::size_t, std::size_t>, std::uint32_t> steps;
	std::deque<std::tuple<char, std::uint32_t, std::pair<std::size_t, std::size_t>>> testPoints;
	auto testFunc1 = [&](char currentHeight, std::uint32_t numSteps, std::pair<std::size_t, std::size_t> testPoint) -> bool
	{
		std::uint32_t testSteps = std::numeric_limits<std::uint32_t>::max();
		if (steps.contains(testPoint))
		{
			testSteps = steps[testPoint];
		}
		return areaMap[testPoint.second][testPoint.first] - currentHeight <= 1 && numSteps < testSteps - 2;
	};

	auto addPointsToQueue = [&](std::pair<std::size_t, std::size_t> point, std::uint32_t numSteps)
	{
		char height = areaMap[point.second][point.first];
		if (point.first > 0)
		{
			testPoints.push_back({ height, numSteps, std::make_pair(point.first - 1, point.second) });
		}
		if (point.second > 0)
		{
			testPoints.push_back({ height, numSteps, std::make_pair(point.first, point.second - 1) });
		}
		if (point.first < areaMap[point.second].size() - 1)
		{
			testPoints.push_back({ height, numSteps, std::make_pair(point.first + 1, point.second) });
		}
		if (point.second < areaMap.size() - 1)
		{
			testPoints.push_back({ height, numSteps, std::make_pair(point.first, point.second + 1) });
		}
	};

	addPointsToQueue(start, 0);

	while (!testPoints.empty())
	{
		auto& [height, numSteps, tPoint] = testPoints.front();
		if (testFunc1(height, numSteps, tPoint))
		{
			steps[tPoint] = numSteps + 1;
			if (tPoint != end)
			{
				addPointsToQueue(tPoint, numSteps + 1);
			}
		}
		testPoints.pop_front();
	}

	std::cout << steps[end] << "\n";

	//Part 2
	steps.clear();
	auto testFunc2 = [&](char currentHeight, std::uint32_t numSteps, std::pair<std::size_t, std::size_t> testPoint) -> bool
	{
		std::uint32_t testSteps = std::numeric_limits<std::uint32_t>::max();
		if (steps.contains(testPoint))
		{
			testSteps = steps[testPoint];
		}
		return currentHeight - areaMap[testPoint.second][testPoint.first] <= 1 && numSteps < testSteps - 2;
	};

	addPointsToQueue(end, 0);
	std::set<std::uint32_t> possibleStartDistances;

	while (!testPoints.empty())
	{
		auto& [height, numSteps, tPoint] = testPoints.front();
		if (testFunc2(height, numSteps, tPoint))
		{
			steps[tPoint] = numSteps + 1;
			if (areaMap[tPoint.second][tPoint.first] == 'a')
			{
				possibleStartDistances.insert(numSteps + 1);
			}
			else
			{
				addPointsToQueue(tPoint, numSteps + 1);
			}
		}
		testPoints.pop_front();
	}

	std::cout << *possibleStartDistances.begin() << "\n";
}
