#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <memory>
#include <regex>
#include <cassert>

class Cavern
{
public:
	Cavern(std::string name, std::uint64_t flowRate) :
		m_Name(name),
		m_FlowRate(flowRate)
	{}

	void AddConnection(std::shared_ptr<Cavern> tunnelTo)
	{
		m_Connections.push_back(tunnelTo);
	}

	void OpenValve()
	{
		m_IsOpened = true;
	}

	void Tick()
	{
		if (m_IsOpened)
		{
			m_PressureReleased += m_FlowRate;
		}
	}

	std::uint64_t GetPressureReleased() const { return m_PressureReleased; }

	std::uint64_t GetScore() const
	{
		return m_IsOpened ? 0 : m_FlowRate;
	}

	std::strong_ordering operator<=>(const Cavern& other)
	{
		if (this->m_Name == other.m_Name)
		{
			return std::strong_ordering::equal;
		}
		if (this->GetScore() == other.GetScore())
		{
			return std::strong_ordering::equivalent;
		}
		return this->GetScore() <=> other.GetScore();
	}

private:
	std::string m_Name;
	std::uint64_t m_FlowRate;
	std::uint64_t m_PressureReleased{ 0 };
	bool m_IsOpened{ false };
	std::vector<std::shared_ptr<Cavern>> m_Connections;
};

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::string> input;
	std::ifstream inStrm(inputFile);

	std::unordered_map<std::string, std::shared_ptr<Cavern>> caverns;
	std::string line;

	std::regex caveName("([A-Z]{2})");
	std::regex flowRate("flow rate=(\\d+)");
	std::smatch m;

	while (std::getline(inStrm, line))
	{
		input.push_back(line);
		std::regex_search(line, m, caveName);
		std::string name = m[1];
		std::regex_search(line, m, flowRate);
		std::uint64_t rate = std::stoull(m[1]);
		caverns[name] = std::make_shared<Cavern>(name, flowRate);
	}

	for (auto caveLine : input)
	{
		std::regex_search(caveLine, m, caveName);
		std::string name = m[1];
		caveLine = m.suffix();

		while (std::regex_search(caveLine, m, caveName))
		{
			assert(caverns.contains(m[1]));
			caverns[name]->AddConnection(caverns[m[1]]);
			caveLine = m.suffix();
		}
	}
}
