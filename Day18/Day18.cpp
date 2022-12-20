#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <sstream>

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
	char c;
	while (inStrm >> x >> c >> y >> c >> z)
	{
		lava.emplace_back(x, y, z);
	}

	std::sort(lava.begin(), lava.end());

	std::size_t surfaceArea1{ 0 };
	for (auto& p : lava)
	{
		std::vector<LavaPoint> diff;
		std::vector<LavaPoint> n = p.GetNeighbours();
		
		std::set_difference(n.begin(), n.end(), lava.begin(), lava.end(), std::back_inserter(diff));

		surfaceArea1 += diff.size();
	}

	std::cout << surfaceArea1 << "\n";
}
