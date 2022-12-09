#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <set>
#include <algorithm>
#include <array>

using Point = std::pair<std::int32_t, std::int32_t>;

void MoveTail(const Point& head, Point& tail)
{
	if ((head.first == tail.first && std::abs(head.second - tail.second) <= 1)
		|| (head.second == tail.second && std::abs(head.first - tail.first) <= 1)
		|| (std::abs(head.second - tail.second) <= 1 && std::abs(head.first - tail.first) <= 1))
	{
		return;
	}

	if (head.first == tail.first)
	{
		if (head.second > tail.second)
		{
			tail.second++;
		}
		else
		{
			tail.second--;
		}
	}
	else if (head.second == tail.second)
	{
		if (head.first > tail.first)
		{
			tail.first++;
		}
		else
		{
			tail.first--;
		}
	}
	else //move diagonally
	{
		if (head.second > tail.second)
		{
			tail.second++;
		}
		else
		{
			tail.second--;
		}

		if (head.first > tail.first)
		{
			tail.first++;
		}
		else
		{
			tail.first--;
		}
	}
}

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	char dir;
	std::int32_t dist;

	Point head{ 0, 0 };
	std::array<Point, 9> tail{};
	std::set<Point> tail1Hist;
	std::set<Point> tail9Hist;
	while (inStrm >> dir >> dist)
	{
		for (std::int32_t i = 0; i < dist; i++)
		{
			switch (dir)
			{
			case 'U':
				head.second++;
				break;
			case 'D':
				head.second--;
				break;
			case 'L':
				head.first--;
				break;
			case 'R':
				head.first++;
				break;
			default:
				throw std::exception("Bad Direction");
				break;
			}

			MoveTail(head, tail[0]);

			for (std::size_t i = 1; i < 9; i++)
			{
				MoveTail(tail[i - 1], tail[i]);
			}

			tail1Hist.insert(tail[0]);
			tail9Hist.insert(tail[8]);
		}
	}

	std::cout << tail1Hist.size() << "\n";
	std::cout << tail9Hist.size() << "\n";
}
