#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <regex>
#include <unordered_map>
#include <queue>
#include <optional>
#include <cassert>

enum class Resource
{
	Ore,
	Clay,
	Obsidian,
	Geode,
};

class RobotFactory
{
	inline static const std::regex m_Parser{ "Each ([^ ]+) robot costs (\\d+) ([^ ]+)( and (\\d+) ([^ ]+))?\\." };
public:
	RobotFactory(std::string recipe)
	{
		std::smatch m;
		while (std::regex_search(recipe, m, m_Parser))
		{
			auto robot = GetResource(m[1]);
			std::int32_t cost1 = std::stoi(m[2]);
			auto resource1 = GetResource(m[3]);
			m_Costs[robot].emplace(resource1, cost1);

			if (m[4].matched)
			{
				std::int32_t cost2 = std::stoi(m[5]);
				auto resource2 = GetResource(m[6]);
				m_Costs[robot].emplace(resource2, cost2);
			}

			recipe = m.suffix();
		}
	}

	~RobotFactory() = default;

	bool CanBuild(Resource type, const std::unordered_map<Resource, std::int32_t> currentResources)
	{
		const auto& cost = m_Costs[type];
		for (const auto& [r, c] : cost)
		{
			if (!currentResources.contains(r) || currentResources.at(r) < c)
			{
				return false;
			}
		}

		return true;
	}

	std::unordered_map<Resource, std::int32_t> BuildRobot(Resource type, const std::unordered_map<Resource, std::int32_t>& currentResources)
	{
		assert(CanBuild(type, currentResources));

		auto ret = currentResources;
		const auto& cost = m_Costs[type];
		for (const auto& [r, c] : cost)
		{
			ret[r] -= c;
		}

		return ret;
	}

private:
	Resource GetResource(const std::string& str)
	{
		if (str == "ore")
		{
			return Resource::Ore;
		}
		else if (str == "clay")
		{
			return Resource::Clay;
		}
		else if (str == "obsidian")
		{
			return Resource::Obsidian;
		}
		else if (str == "geode")
		{
			return Resource::Geode;
		}
	}

	std::unordered_map<Resource, std::unordered_map<Resource, std::int32_t>> m_Costs;
};


int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	std::vector<RobotFactory> factories;

	std::string line;
	while (std::getline(inStrm, line))
	{
		factories.emplace_back(line);
	}

	std::vector<std::uint32_t> bestScore(factories.size(), 0);
	using State = std::tuple<std::unordered_map<Resource, std::int32_t>, std::unordered_map<Resource, std::int32_t>, std::optional<Resource>, std::uint32_t>;

	const std::vector<std::optional<Resource>> buildOptions{ Resource::Geode, Resource::Obsidian, Resource::Clay, Resource::Ore, std::nullopt };

	for (std::size_t i{ 0 }; i < factories.size(); i++)
	{
		std::queue<State> fillQueue;
		std::unordered_map<Resource, std::int32_t> robots;
		robots.emplace(Resource::Ore, 1);
		std::unordered_map<Resource, std::int32_t> resources;
		std::optional<Resource> botBeingBuilt{ std::nullopt };
		std::uint32_t timeSpent{ 0 };

		fillQueue.push(std::make_tuple(robots, resources, botBeingBuilt, timeSpent));
		
		while (!fillQueue.empty())
		{
			State nextState = fillQueue.front();
			std::tie(robots, resources, botBeingBuilt, timeSpent) = nextState;

			for (auto [r, c] : robots)
			{
				resources[r] += c;
			}

			if (timeSpent == 24)
			{
				if (resources[Resource::Geode] > bestScore[i])
				{
					bestScore[i] = resources[Resource::Geode];
				}
			}
			else
			{
				if (botBeingBuilt.has_value())
				{
					robots[botBeingBuilt.value()]++;
				}

				for (auto buildBot : buildOptions)
				{
					if (!buildBot.has_value())
					{
						fillQueue.push(std::make_tuple(robots, resources, buildBot, timeSpent + 1));
					}
					else if (factories[i].CanBuild(buildBot.value(), resources))
					{
						auto nextResources = factories[i].BuildRobot(buildBot.value(), resources);

						fillQueue.push(std::make_tuple(robots, nextResources, buildBot, timeSpent + 1));
					}
				}
			}

			fillQueue.pop();
		}
	}

	std::size_t score{ 0 };
	for (std::size_t i{ 0 }; i < bestScore.size(); i++)
	{
		score += (i + 1) * bestScore[i];
	}

	std::cout << score << "\n";
}
