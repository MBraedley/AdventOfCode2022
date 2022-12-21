#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <regex>
#include <optional>
#include <deque>
#include <set>

using Coords = std::pair<std::int64_t, std::int64_t>;

std::int64_t GetManhattanDist(const Coords& sensor, const Coords& beacon)
{
	return std::abs(sensor.first - beacon.first) + std::abs(sensor.second - beacon.second);
}

std::optional<std::pair<std::int64_t, std::int64_t>> GetRowPoints(const Coords& sensor, const Coords& beacon, std::int64_t row = 2000000)
{
	std::int64_t closest = GetManhattanDist(sensor, std::make_pair(sensor.first, row));
	std::int64_t md = GetManhattanDist(sensor, beacon);

	if (closest > md)
	{
		return std::nullopt;
	}

	std::int64_t extra = std::abs(closest - md);
	std::int64_t left = sensor.first - extra;
	std::int64_t right = sensor.first + extra;

	return std::make_pair(left, right);
}

void ReduceRow(std::deque<std::pair<std::int64_t, std::int64_t>>& row)
{
	bool didMerge = false;
	std::deque<std::pair<std::int64_t, std::int64_t>> next;
	std::pair<std::int64_t, std::int64_t> comp1 = row.front();
	row.pop_front();

	while (!row.empty())
	{
		std::pair<std::int64_t, std::int64_t> comp2 = row.front();
		row.pop_front();

		if (comp2.first <= comp1.second)
		{
			comp1.second = std::max(comp1.second, comp2.second);
			didMerge = true;
		}
		else
		{
			next.push_back(comp1);
			comp1 = comp2;
		}
	}

	next.push_back(comp1);

	std::swap(row, next);

	if (didMerge)
	{
		ReduceRow(row);
	}
}

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::regex findCoords("x=(-?\\d+), y=(-?\\d+)");
	std::smatch m;
	
	std::vector<std::pair<Coords, Coords>> sensorsAndBeacons;
	std::set<Coords> justBeacons;

	std::string line;
	while (std::getline(inStrm, line))
	{
		std::regex_search(line, m, findCoords);
		Coords sensor = std::make_pair(std::stoi(m[1]), std::stoi(m[2]));
		line = m.suffix();
		std::regex_search(line, m, findCoords);
		Coords beacon = std::make_pair(std::stoi(m[1]), std::stoi(m[2]));

		sensorsAndBeacons.emplace_back(sensor, beacon);
		justBeacons.insert(beacon);
	}

	std::deque<std::pair<std::int64_t, std::int64_t>> coveredPoints20000;

	for (const auto& [sensor, beacon] : sensorsAndBeacons)
	{
		if (auto covered = GetRowPoints(sensor, beacon); covered != std::nullopt)
		{
			coveredPoints20000.push_back(covered.value());
		}
	}

	std::sort(coveredPoints20000.begin(), coveredPoints20000.end(), [](const Coords& lhs, const Coords& rhs)
		{
			return lhs.first < rhs.first;
		});

	ReduceRow(coveredPoints20000);
	std::int64_t numPoints = 0;

	for (const auto& [left, right] : coveredPoints20000)
	{
		numPoints += right - left + 1;
	}

	for (const auto& beacon : justBeacons)
	{
		if (beacon.second == 2000000)
		{
			numPoints--;
		}
	}

	std::cout << numPoints << "\n";

	//Part 2
	for (std::int64_t y = 0; y <= 4000000; y++)
	{
		std::deque<std::pair<std::int64_t, std::int64_t>> coveredPoints;
		for (const auto& [sensor, beacon] : sensorsAndBeacons)
		{
			if (auto covered = GetRowPoints(sensor, beacon, y); covered != std::nullopt)
			{
				coveredPoints.emplace_back(covered.value());
			}
		}

		std::sort(coveredPoints.begin(), coveredPoints.end(), [](const Coords& lhs, const Coords& rhs)
			{
				return lhs.first < rhs.first;
			});

		if (coveredPoints.empty())
		{
			throw std::exception("Expected points");
		}

		ReduceRow(coveredPoints);
		if (coveredPoints.front() != coveredPoints.back())
		{
			std::int64_t x = coveredPoints.front().second + 1;
			std::cout << x * 4000000 + y << "\n";
			return 0;
		}
	}
}
