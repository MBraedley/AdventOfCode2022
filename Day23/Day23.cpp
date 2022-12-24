#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <deque>
#include <numeric>
#include <cassert>

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

	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::map<std::uint32_t, Point> elves2Positions;
	std::map<Point, std::uint32_t> positions2Elves;
	std::map<Point, std::set<std::uint32_t>> plannedMovements;

	std::uint32_t elfId{ 0 };
	std::int32_t y{ 0 };
	std::string line;
	while (std::getline(inStrm, line))
	{
		for (std::int32_t x = 0; x < line.size(); x++)
		{
			if (line[x] == '#')
			{
				elves2Positions.emplace(elfId, std::make_tuple(x, y));
				positions2Elves.emplace(std::make_tuple(x, y), elfId);
				elfId++;
			}
		}
		y++;
	}

	enum MoveDir
	{
		North = 0,
		South,
		West,
		East,
	};

	std::deque<MoveDir> movePrefs{ North, South, West, East };

	auto IsCrowded = [&](std::uint32_t elfId) -> bool
	{
		Point p = elves2Positions[elfId];
		return positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p) - 1))
			|| positions2Elves.contains(std::make_tuple(X(p), Y(p) - 1))
			|| positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p) - 1))
			|| positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p)))
			|| positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p) + 1))
			|| positions2Elves.contains(std::make_tuple(X(p), Y(p) + 1))
			|| positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p) + 1))
			|| positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p)));
	};

	auto CanMove = [&](std::uint32_t elfId, MoveDir dir) -> bool
	{
		Point p = elves2Positions[elfId];
		switch (dir)
		{
		case North:
			return !positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p) - 1))
				&& !positions2Elves.contains(std::make_tuple(X(p), Y(p) - 1))
				&& !positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p) - 1));
		case South:
			return !positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p) + 1))
				&& !positions2Elves.contains(std::make_tuple(X(p), Y(p) + 1))
				&& !positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p) + 1));
		case West:
			return !positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p) - 1))
				&& !positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p)))
				&& !positions2Elves.contains(std::make_tuple(X(p) - 1, Y(p) + 1));
		case East:
			return !positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p) - 1))
				&& !positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p)))
				&& !positions2Elves.contains(std::make_tuple(X(p) + 1, Y(p) + 1));
		default:
			assert(false);
			return false;
		}
	};

	auto PlanMove = [&](std::uint32_t elfId)
	{
		for (auto dir : movePrefs)
		{
			if (CanMove(elfId, dir))
			{
				Point p = elves2Positions[elfId];
				switch (dir)
				{
				case North:
					Y(p)--;
					break;
				case South:
					Y(p)++;
					break;
				case West:
					X(p)--;
					break;
				case East:
					X(p)++;
					break;
				default:
					assert(false);
					break;
				}

				plannedMovements[p].insert(elfId);
				return;
			}
		}
	};

	auto MoveElves = [&]()
	{
		assert(positions2Elves.size() == elves2Positions.size());
		for (auto& [p, elves] : plannedMovements)
		{
			if (elves.size() == 1)
			{
				auto elf = *elves.begin();
				Point lastPos = elves2Positions[elf];
				positions2Elves.erase(lastPos);

				positions2Elves[p] = elf;
				elves2Positions[elf] = p;
			}
		}

		assert(positions2Elves.size() == elves2Positions.size());

		movePrefs.push_back(movePrefs.front());
		movePrefs.pop_front();
	};

	for (auto i = 0; i < 10; i++)
	{
		plannedMovements.clear();
		for (auto& [elf, pos] : elves2Positions)
		{
			if (IsCrowded(elf))
			{
				PlanMove(elf);
			}
		}
		MoveElves();
	}

	std::int32_t top = std::numeric_limits<std::int32_t>::max();
	std::int32_t left = std::numeric_limits<std::int32_t>::max();
	std::int32_t bottom = std::numeric_limits<std::int32_t>::min();
	std::int32_t right = std::numeric_limits<std::int32_t>::min();

	for (auto& [elf, p] : elves2Positions)
	{
		if (X(p) < left)
		{
			left = X(p);
		}
		if (X(p) > right)
		{
			right = X(p);
		}
		if (Y(p) < top)
		{
			top = Y(p);
		}
		if (Y(p) > bottom)
		{
			bottom = Y(p);
		}
	}

	std::cout << (right - left + 1) * (bottom - top + 1) - elves2Positions.size() << "\n";

	//Part 2
	bool elfMoved = true;
	std::uint32_t roundsComplete = 10;

	while (elfMoved)
	{
		plannedMovements.clear();
		for (auto& [elf, pos] : elves2Positions)
		{
			if (IsCrowded(elf))
			{
				PlanMove(elf);
			}
		}

		elfMoved = !plannedMovements.empty();
		MoveElves();

		roundsComplete++;
	}

	std::cout << roundsComplete << "\n";
}
