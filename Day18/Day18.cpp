#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <queue>
#include <numeric>
#include <cassert>
#include <set>

class LavaPoint
{
public:
	LavaPoint(std::int32_t x, std::int32_t y, std::int32_t z) :
		m_X(x),
		m_Y(y),
		m_Z(z)
	{}

	std::string ToString() const
	{
		std::stringstream sstrm;
		sstrm << m_X << "," << m_Y << "," << m_Z;
		return sstrm.str();
	}

	std::vector<LavaPoint> GetNeighbours() const
	{
		std::vector<LavaPoint> ret;
		ret.emplace_back(m_X - 1, m_Y, m_Z);
		ret.emplace_back(m_X, m_Y - 1, m_Z);
		ret.emplace_back(m_X, m_Y, m_Z - 1);
		ret.emplace_back(m_X, m_Y, m_Z + 1);
		ret.emplace_back(m_X, m_Y + 1, m_Z);
		ret.emplace_back(m_X + 1, m_Y, m_Z);

		return ret;
	}

	auto operator<=>(const LavaPoint& other) const
	{
		if (this->m_X == other.m_X)
		{
			if (this->m_Y == other.m_Y)
			{
				return this->m_Z <=> other.m_Z;
			}
			return this->m_Y <=> other.m_Y;
		}
		return this->m_X <=> other.m_X;
	}

	std::int32_t m_X;
	std::int32_t m_Y;
	std::int32_t m_Z;
};

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<LavaPoint> lava;
	std::ifstream inStrm(inputFile);

	std::int32_t x, y, z;
	LavaPoint minCorner(std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max());
	LavaPoint maxCorner(std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::min());
	char c;
	while (inStrm >> x >> c >> y >> c >> z)
	{
		lava.emplace_back(x, y, z);

		// Prep for part 2
		if (x < minCorner.m_X)
		{
			minCorner.m_X = x;
		}
		if (y < minCorner.m_Y)
		{
			minCorner.m_Y = y;
		}
		if (z < minCorner.m_Z)
		{
			minCorner.m_Z = z;
		}
		if (x > maxCorner.m_X)
		{
			maxCorner.m_X = x;
		}
		if (y > maxCorner.m_Y)
		{
			maxCorner.m_Y = y;
		}
		if (z > maxCorner.m_Z)
		{
			maxCorner.m_Z = z;
		}
	}

	std::sort(lava.begin(), lava.end());

	std::vector<LavaPoint> faces;
	
	for (auto& p : lava)
	{
		std::vector<LavaPoint> n = p.GetNeighbours();
		
		std::set_difference(n.begin(), n.end(), lava.begin(), lava.end(), std::back_inserter(faces));
	}

	std::cout << faces.size() << "\n";

	//Part 2
	std::vector<LavaPoint> exteriorFaces;
	
	//Need an air block in order to count the faces properly
	minCorner.m_X--;
	minCorner.m_Y--;
	minCorner.m_Z--;
	maxCorner.m_X++;
	maxCorner.m_Y++;
	maxCorner.m_Z++;

	std::queue<LavaPoint> bfsQueue;
	bfsQueue.push(minCorner);

	std::set<LavaPoint> visited;
	visited.emplace(minCorner);

	auto inBounds = [&](LavaPoint& point) -> bool
	{
		return point.m_X >= minCorner.m_X
			&& point.m_Y >= minCorner.m_Y
			&& point.m_Z >= minCorner.m_Z
			&& point.m_X <= maxCorner.m_X
			&& point.m_Y <= maxCorner.m_Y
			&& point.m_Z <= maxCorner.m_Z;
	};

	while (!bfsQueue.empty())
	{
		LavaPoint testPoint = bfsQueue.front();
		bfsQueue.pop();
		std::vector<LavaPoint> n = testPoint.GetNeighbours();
		std::set_intersection(n.begin(), n.end(), lava.begin(), lava.end(), std::back_inserter(exteriorFaces));

		std::vector<LavaPoint> airBlocks;
		std::set_difference(n.begin(), n.end(), lava.begin(), lava.end(), std::back_inserter(airBlocks));

		for (LavaPoint& step : airBlocks)
		{
			if (!visited.contains(step) && inBounds(step))
			{
				bfsQueue.push(step);
				visited.emplace(step);
			}
		}
	}
	std::cout << "- " << exteriorFaces.size() << "\n"
		<< "= " << faces.size() - exteriorFaces.size() << "\n";
}
