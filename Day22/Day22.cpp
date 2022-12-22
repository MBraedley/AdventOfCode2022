#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <regex>
#include <cassert>

enum Facing
{
	Right = 0,
	Down,
	Left,
	Up,
};

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
	std::vector<std::string> map;
	std::ifstream inStrm(inputFile);

	std::string line;
	while (std::getline(inStrm, line) && !line.empty())
	{
		map.push_back(line);
	}

	std::string instructions;
	std::getline(inStrm, instructions);

	std::string instPt2 = instructions;	//because I'm going to have to save this, I just know it.

	std::regex instRegex("(\\d+)(L|R|)");
	std::smatch m;

	std::int32_t part = 1;

	auto Part2EdgeTraversal = [&](Point& p, Facing& f)
	{
		switch (f)
		{
		case Right:
		{
			if (Y(p) < 50)
			{
				assert(X(p) == 150);
				X(p) = 99;
				Y(p) = 149 - Y(p);
				f = Facing::Left;
			}
			else if (Y(p) < 100)
			{
				assert(X(p) == 100);
				X(p) = Y(p) + 50;
				Y(p) = 49;
				f = Facing::Up;
			}
			else if (Y(p) < 150)
			{
				assert(X(p) == 100);
				X(p) = 149;
				Y(p) = 149 - Y(p);
				f = Facing::Left;
			}
			else
			{
				assert(X(p) == 50);
				X(p) = Y(p) - 100;
				Y(p) = 149;
				f = Facing::Up;
			}
			break;
		}
		case Down:
		{
			if (X(p) < 50)
			{
				assert(Y(p) == 200);
				Y(p) = 0;
				X(p) = X(p) + 100;
			}
			else if (X(p) < 100)
			{
				assert(Y(p) == 150);
				Y(p) = X(p) + 100;
				X(p) = 49;
				f = Facing::Left;
			}
			else
			{
				assert(Y(p) == 50);
				Y(p) = X(p) - 50;
				X(p) = 99;
				f = Facing::Left;
			}
			break;
		}
		case Left:
		{
			if (Y(p) < 50)
			{
				assert(X(p) == 49);
				X(p) = 0;
				Y(p) = 149 - Y(p);
				f = Facing::Right;
			}
			else if (Y(p) < 100)
			{
				assert(X(p) == 49);
				X(p) = Y(p) - 50;
				Y(p) = 100;
				f = Facing::Down;
			}
			else if (Y(p) < 150)
			{
				assert(X(p) == -1);
				X(p) = 50;
				Y(p) = 149 - Y(p);
				f = Facing::Right;
			}
			else
			{
				assert(X(p) == -1);
				X(p) = Y(p) - 100;
				Y(p) = 0;
				f = Facing::Down;
			}
			break;
		}
		case Up:
		{
			if (X(p) < 50)
			{
				assert(Y(p) == 99);
				Y(p) = X(p) + 50;
				X(p) = 50;
				f = Facing::Right;
			}
			else if (X(p) < 100)
			{
				assert(Y(p) = -1);
				Y(p) = X(p) + 100;
				X(p) = 0;
				f = Facing::Right;
			}
			else
			{
				assert(Y(p) = -1);
				Y(p) = 199;
				X(p) = X(p) - 100;
			}
			break;
		}
		default:
			assert(false);
			break;
		}
	};

	auto NextBlock = [&](const Point& p, Facing f) -> std::tuple<bool, Point, Facing>
	{
		Point ret = p;
		switch (f)
		{
		case Right:
			X(ret)++;
			break;
		case Down:
			Y(ret)++;
			break;
		case Left:
			X(ret)--;
			break;
		case Up:
			Y(ret)--;
			break;
		default:
			assert(false);
			break;
		}

		if (Y(ret) >= 0 && Y(ret) < map.size() && X(ret) >= 0 && X(ret) < map[Y(ret)].size() && map[Y(ret)][X(ret)] != ' ')
		{
			return std::make_tuple(map[Y(ret)][X(ret)] == '.', ret, f);
		}

		if (part == 1)
		{
			//Easy case done, need to wrap around now
			switch (f)
			{
			case Right:
				X(ret) = map[Y(ret)].find_first_not_of(' ');
				break;
			case Down:
			{
				while (Y(ret) > 0 && map[Y(ret) - 1][X(ret)] != ' ')
				{
					Y(ret)--;
				}
				break;
			}
			case Left:
				X(ret) = map[Y(ret)].find_last_not_of(' ');
				break;
			case Up:
			{
				while (Y(ret) + 1 < map.size() && X(ret) < map[Y(ret) + 1].size() && map[Y(ret) + 1][X(ret)] != ' ')
				{
					Y(ret)++;
				}
				break;
			}
			default:
				assert(false);
				break;
			}
		}
		else
		{
			Part2EdgeTraversal(ret, f);
		}

		return std::make_tuple(map[Y(ret)][X(ret)] == '.', ret, f);
	};

	for ( ; part <= 2; part++ )
	{
		Point pos;
		Y(pos) = 0;
		X(pos) = map[0].find_first_not_of(' ');
		assert(map[Y(pos)][X(pos)] != '#');

		Facing dir = Facing::Right;

		while (std::regex_search(instructions, m, instRegex))
		{
			std::int32_t dist = std::stoi(m[1]);
			bool openSpot;
			Point nextPos;
			Facing nextDir;
			std::tie(openSpot, nextPos, nextDir) = NextBlock(pos, dir);
			while (dist > 0 && openSpot)
			{
				dist--;
				pos = nextPos;
				dir = nextDir;
				std::tie(openSpot, nextPos, nextDir) = NextBlock(pos, dir);
			}

			if (m[2].str() == "R")
			{
				switch (dir)
				{
				case Right:
					dir = Facing::Down;
					break;
				case Down:
					dir = Facing::Left;
					break;
				case Left:
					dir = Facing::Up;
					break;
				case Up:
					dir = Facing::Right;
					break;
				default:
					assert(false);
					break;
				}
			}
			else if (m[2].str() == "L")
			{
				switch (dir)
				{
				case Right:
					dir = Facing::Up;
					break;
				case Down:
					dir = Facing::Right;
					break;
				case Left:
					dir = Facing::Down;
					break;
				case Up:
					dir = Facing::Left;
					break;
				default:
					assert(false);
					break;
				}
			}
			else
			{
				std::cout << "No Turn\n";
			}

			instructions = m.suffix();
		}

		std::cout << 1000 * (Y(pos) + 1) + 4 * (X(pos) + 1) + dir << "\n";

		instructions = instPt2;
	}
}
