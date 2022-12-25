#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <set>

int main()
{
	using Point = std::tuple<std::int32_t, std::int32_t>;
	auto X = [](Point& p) -> std::int32_t&
	{
		return std::get<0>(p);
	};
	auto Y = [](Point& p) -> std::int32_t&
	{
		return std::get<1>(p);
	};

	enum Dir
	{
		North,
		South,
		East,
		West,
	};

	struct Blizzard
	{
		Point pos;
		Dir dir;
	};

	std::vector<Blizzard> blizzards;

	std::filesystem::path inputFile("input.txt");
	std::vector<std::string> input;
	std::ifstream inStrm(inputFile);

	std::string line;
	std::uint64_t calCount = 0;
	while (std::getline(inStrm, line))
	{
		input.push_back(line);
	}

	Point start;
	Point end;
	Point bottomRight;

	Point goal;

	for (std::int32_t i{ 0 }; i < input.size(); i++)
	{
		if (i == 0)
		{
			X(start) = input[i].find_first_not_of('#');
			Y(start) = 0;
		}
		else if (i == input.size() - 1)
		{
			X(end) = input[i].find_last_not_of('#');
			Y(end) = i;
			X(bottomRight) = input[i].size() - 1;
			Y(bottomRight) = i;
		}
		else
		{
			for (std::int32_t x = 0; x < input[i].size(); x++)
			{

				switch (input[i][x])
				{
				case '^':
					blizzards.emplace_back(std::make_tuple(x, i), Dir::North);
					break;
				case 'v':
					blizzards.emplace_back(std::make_tuple(x, i), Dir::South);
					break;
				case '>':
					blizzards.emplace_back(std::make_tuple(x, i), Dir::East);
					break;
				case '<':
					blizzards.emplace_back(std::make_tuple(x, i), Dir::West);
					break;
				default:
					break;
				}
			}
		}
	}

	auto stepBlizzards = [&]() -> std::set<Point>
	{
		std::set<Point> ret;
		for (auto& b : blizzards)
		{
			switch (b.dir)
			{
			case North:
				Y(b.pos)--;
				if (Y(b.pos) == 0)
				{
					Y(b.pos) = Y(bottomRight) - 1;
				}
				break;
			case South:
				Y(b.pos)++;
				if (Y(b.pos) == Y(bottomRight))
				{
					Y(b.pos) = 1;
				}
				break;
			case East:
				X(b.pos)++;
				if (X(b.pos) == X(bottomRight))
				{
					X(b.pos) = 1;
				}
				break;
			case West:
				X(b.pos)--;
				if (X(b.pos) == 0)
				{
					X(b.pos) = X(bottomRight) - 1;
				}
				break;
			default:
				break;
			}

			ret.insert(b.pos);
		}
		return ret;
	};

	auto getNeighbours = [&](Point pos) -> std::vector<Point>
	{
		std::vector<Point> ret;
		ret.push_back(pos);

		if (pos == start && goal == end)	//Special case for the start
		{
			ret.push_back(std::make_tuple(X(pos), Y(pos) + 1));
			return ret;
		}

		if (pos == end && goal == start)
		{
			ret.push_back(std::make_tuple(X(pos), Y(pos) - 1));
			return ret;
		}

		auto north = std::make_tuple(X(pos), Y(pos) - 1);
		if (north == goal)
		{
			return { north };
		}
		if (Y(north) >= 1)
		{
			ret.push_back(north);
		}

		auto south = std::make_tuple(X(pos), Y(pos) + 1);
		if (south == goal)	//Special case for the end
		{
			return { south };
		}
		if (Y(south) < Y(bottomRight))
		{
			ret.push_back(south);
		}

		auto east = std::make_tuple(X(pos) + 1, Y(pos));
		if (X(east) < X(bottomRight))
		{
			ret.push_back(east);
		}

		auto west = std::make_tuple(X(pos) - 1, Y(pos));
		if (X(west) >= 1)
		{
			ret.push_back(west);
		}

		return ret;
	};

	bool endReached = false;
	std::size_t timeTaken{ 0 };
	std::set<Point> quantumElves;
	quantumElves.insert(start);

	auto runRoute = [&]()
	{
		while (!endReached)
		{
			std::set<Point> nextQuantumPos;
			std::set<Point> nextBlizzarPos = stepBlizzards();
			for (auto& pos : quantumElves)
			{
				for (auto& nPos : getNeighbours(pos))
				{
					if (nPos == goal)
					{
						endReached = true;
						break;
					}
					else if (!nextBlizzarPos.contains(nPos))
					{
						nextQuantumPos.insert(nPos);
					}
				}
			}

			timeTaken++;
			std::swap(quantumElves, nextQuantumPos);
		}
	};

	goal = end;
	runRoute();

	std::cout << timeTaken << "\n";

	//Part 2
	goal = start;
	endReached = false;
	quantumElves.clear();
	quantumElves.insert(end);
	runRoute();

	goal = end;
	endReached = false;
	quantumElves.clear();
	quantumElves.insert(start);
	runRoute();

	std::cout << timeTaken << "\n";
}
