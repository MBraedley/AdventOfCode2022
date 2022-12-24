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
#include <future>
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

			if (cost1 > m_MaxCosts[resource1])
			{
				m_MaxCosts[resource1] = cost1;
			}

			if (m[4].matched)
			{
				std::int32_t cost2 = std::stoi(m[5]);
				auto resource2 = GetResource(m[6]);
				m_Costs[robot].emplace(resource2, cost2);

				if (cost2 > m_MaxCosts[resource2])
				{
					m_MaxCosts[resource2] = cost2;
				}
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

	bool ShouldBuild(Resource type, std::int32_t botCount, std::int32_t resourceCount, std::int32_t timeLeft)
	{
		if (type == Resource::Geode)
		{
			return true;	//Always build geode bots, even if they don't get used
		}

		auto maxCost = m_MaxCosts[type];

		return maxCost * timeLeft > resourceCount + botCount * timeLeft;
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
	std::unordered_map<Resource, std::int32_t> m_MaxCosts;
};

std::partial_ordering operator<=>(std::unordered_map<Resource, std::int32_t>& lhs, std::unordered_map<Resource, std::int32_t>& rhs)
{
	if (lhs[Resource::Ore] == rhs[Resource::Ore] &&
		lhs[Resource::Clay] == rhs[Resource::Clay] &&
		lhs[Resource::Obsidian] == rhs[Resource::Obsidian] &&
		lhs[Resource::Geode] == rhs[Resource::Geode])
	{
		return std::partial_ordering::equivalent;
	}
	else if (lhs[Resource::Ore] <= rhs[Resource::Ore] &&
		lhs[Resource::Clay] <= rhs[Resource::Clay] &&
		lhs[Resource::Obsidian] <= rhs[Resource::Obsidian] &&
		lhs[Resource::Geode] <= rhs[Resource::Geode])
	{
		return std::partial_ordering::less;
	}
	else if (lhs[Resource::Ore] >= rhs[Resource::Ore] &&
		lhs[Resource::Clay] >= rhs[Resource::Clay] &&
		lhs[Resource::Obsidian] >= rhs[Resource::Obsidian] &&
		lhs[Resource::Geode] >= rhs[Resource::Geode])
	{
		return std::partial_ordering::greater;
	}
	else
	{
		return std::partial_ordering::unordered;
	}
}

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

	std::vector<std::future<std::uint32_t>> scoreFutures;
	using State = std::tuple<std::unordered_map<Resource, std::int32_t>, std::unordered_map<Resource, std::int32_t>, std::optional<Resource>, std::uint32_t>;

	const std::vector<std::optional<Resource>> buildOptions{ Resource::Geode, Resource::Obsidian, Resource::Clay, Resource::Ore, std::nullopt };

	auto stateCompLess = [](const State& lhs, const State& rhs)
	{
		auto [lRobots, lResources, lBuild, lTime] = lhs;
		auto [rRobots, rResources, rBuild, rTime] = rhs;

		if (lBuild.has_value())
		{
			lRobots[lBuild.value()]++;
		}
		if (rBuild.has_value())
		{
			rRobots[rBuild.value()]++;
		}

		if (lTime == rTime)
		{
			if (lRobots == rRobots)
			{
				return lResources < rResources;
			}
			return lRobots < rRobots;
		}
		return lTime > rTime;
	};

	for (std::size_t i{ 0 }; i < factories.size(); i++)
	{
		 scoreFutures.push_back( std::async(std::launch::async, [&](std::size_t index) -> std::uint32_t
			{
				std::uint32_t bestScore{ 0 };
				std::priority_queue<State, std::vector<State>, decltype(stateCompLess)> fillQueue(stateCompLess);
				std::unordered_map<Resource, std::int32_t> robots;
				robots.emplace(Resource::Ore, 1);
				std::unordered_map<Resource, std::int32_t> resources;
				std::optional<Resource> botBeingBuilt{ std::nullopt };
				std::uint32_t timeSpent{ 1 };

				fillQueue.push(std::make_tuple(robots, resources, botBeingBuilt, timeSpent));

				std::vector<State> statesThisMinute;

				while (!fillQueue.empty())
				{
					State nextState = fillQueue.top();
					std::tie(robots, resources, botBeingBuilt, timeSpent) = nextState;

					fillQueue.pop();

					for (auto [r, c] : robots)
					{
						resources[r] += c;
					}

					if (botBeingBuilt.has_value())
					{
						robots[botBeingBuilt.value()]++;
						botBeingBuilt = std::nullopt;
					}

					bool prune = false;

					if (statesThisMinute.empty() || std::get<3>(statesThisMinute[0]) != timeSpent)
					{
						statesThisMinute.clear();
					}
					else
					{
						for (auto& [oRobots, oResources, oBuild, oTime] : statesThisMinute)
						{
							auto robotComp = robots <=> oRobots;
							auto resourceComp = resources <=> oResources;
							if ((robotComp == std::partial_ordering::less || robotComp == std::partial_ordering::equivalent)
								&& (resourceComp == std::partial_ordering::less || resourceComp == std::partial_ordering::equivalent))
							{
								prune = true;
								break;
							}
						}
					}

					if (timeSpent == 24)
					{
						if (resources[Resource::Geode] > bestScore)
						{
							bestScore = resources[Resource::Geode];
						}
					}
					else if (!prune)
					{
						statesThisMinute.push_back(nextState);

						for (auto buildBot : buildOptions)
						{
							if (!buildBot.has_value())
							{
								fillQueue.push(std::make_tuple(robots, resources, buildBot, timeSpent + 1));
							}
							else if (factories[index].CanBuild(buildBot.value(), resources))
							{
								if (factories[index].ShouldBuild(buildBot.value(), robots[buildBot.value()], resources[buildBot.value()], 24 - timeSpent))
								{
									auto nextResources = factories[index].BuildRobot(buildBot.value(), resources);

									fillQueue.push(std::make_tuple(robots, nextResources, buildBot, timeSpent + 1));
								}
								else
								{
									auto t = timeSpent;
								}
							}
						}
					}
				}

				return bestScore;
			}, i) );
	}

	std::size_t score{ 0 };
	std::vector<std::uint32_t> bestScore;
	for (auto& f : scoreFutures)
	{
		bestScore.push_back(f.get());
	}

	for (std::size_t i{ 0 }; i < bestScore.size(); i++)
	{
		score += (i + 1) * bestScore[i];
	}

	for (auto s : bestScore)
	{
		std::cout << s << " ";
	}
	std::cout << score << "\n";
}
