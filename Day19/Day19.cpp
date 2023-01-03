#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <regex>
#include <unordered_map>
#include <deque>
#include <optional>
#include <future>
#include <sstream>
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

	bool CanBuild(Resource type, const std::unordered_map<Resource, std::int32_t> currentResources) const
	{
		const auto& cost = m_Costs.at(type);
		for (const auto& [r, c] : cost)
		{
			if (!currentResources.contains(r) || currentResources.at(r) < c)
			{
				return false;
			}
		}

		return true;
	}

	bool ShouldBuild(Resource type, std::int32_t botCount, std::int32_t resourceCount, std::int32_t timeLeft) const
	{
		if (type == Resource::Geode)
		{
			return true;	//Always build geode bots, even if they don't get used
		}

		if (timeLeft <= 1)
		{
			return false;	//Won't be built in time
		}

		auto maxCost = m_MaxCosts.at(type);

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
	Resource GetResource(const std::string& str) const
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

std::partial_ordering operator<=>(const std::unordered_map<Resource, std::int32_t>& lhs, const std::unordered_map<Resource, std::int32_t>& rhs)
{
	if (lhs.at(Resource::Ore) == rhs.at(Resource::Ore) &&
		lhs.at(Resource::Clay) == rhs.at(Resource::Clay) &&
		lhs.at(Resource::Obsidian) == rhs.at(Resource::Obsidian) &&
		lhs.at(Resource::Geode) == rhs.at(Resource::Geode))
	{
		return std::partial_ordering::equivalent;
	}
	else if (lhs.at(Resource::Ore) <= rhs.at(Resource::Ore) &&
		lhs.at(Resource::Clay) <= rhs.at(Resource::Clay) &&
		lhs.at(Resource::Obsidian) <= rhs.at(Resource::Obsidian) &&
		lhs.at(Resource::Geode) <= rhs.at(Resource::Geode))
	{
		return std::partial_ordering::less;
	}
	else if (lhs.at(Resource::Ore) >= rhs.at(Resource::Ore) &&
		lhs.at(Resource::Clay) >= rhs.at(Resource::Clay) &&
		lhs.at(Resource::Obsidian) >= rhs.at(Resource::Obsidian) &&
		lhs.at(Resource::Geode) >= rhs.at(Resource::Geode))
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

	using ResourceCount = std::unordered_map<Resource, std::int32_t>;
	using RobotCount = ResourceCount;
	using State = std::tuple<RobotCount, ResourceCount, std::optional<Resource>, std::uint32_t>;

	const std::vector<std::optional<Resource>> buildOptions{ std::nullopt, Resource::Ore, Resource::Clay, Resource::Obsidian };

	auto maxPotentialGeodes = [](std::int32_t bots, std::int32_t geodes, std::int32_t timeLeft)
	{
		auto triNum = timeLeft * (timeLeft + 1) / 2;
		return geodes + bots * timeLeft + triNum;
	};

	auto runBlueprint = [&](RobotFactory& factory, std::int32_t runTime) -> std::uint32_t
	{
		std::uint32_t localScore{ 0 };
		std::deque<State> fillQueue;
		RobotCount robots
		{
			{Resource::Ore, 1},
			{Resource::Clay, 0},
			{Resource::Obsidian, 0},
			{Resource::Geode, 0},
		};
		ResourceCount resources
		{
			{Resource::Ore, 0},
			{Resource::Clay, 0},
			{Resource::Obsidian, 0},
			{Resource::Geode, 0},
		};
		std::optional<Resource> botBeingBuilt{ std::nullopt };
		std::uint32_t timeSpent{ 1 };

		std::vector<std::tuple<RobotCount, ResourceCount, std::uint32_t>> bestStates;

		fillQueue.push_front(std::make_tuple(robots, resources, botBeingBuilt, timeSpent));

		while (!fillQueue.empty())
		{
			State nextState = fillQueue.front();
			std::tie(robots, resources, botBeingBuilt, timeSpent) = nextState;

			fillQueue.pop_front();

			for (auto [r, c] : robots)
			{
				resources[r] += c;
			}

			if (botBeingBuilt.has_value())
			{
				robots[botBeingBuilt.value()]++;
				botBeingBuilt = std::nullopt;
			}

			//This is our state at the end of the minute.  Now need to prep for the start of the next minute.
			bool prune = false;
			bool skipInsert = false;
			if (maxPotentialGeodes(robots[Resource::Geode], resources[Resource::Geode], runTime - timeSpent) < localScore)
			{
				//We can't possibly beat best score, so don't bother continuing down this path
				prune = true;
			}
			else
			{
				for (auto& [oRobots, oResources, oTime] : bestStates)
				{
					auto orderRobots = robots <=> oRobots;
					auto orderResources = resources <=> oResources;
					if ((orderRobots == std::partial_ordering::less || orderRobots == std::partial_ordering::equivalent)
						&& (orderResources == std::partial_ordering::less || orderResources == std::partial_ordering::equivalent)
						&& timeSpent >= oTime)
					{
						prune = true;
						break;
					}
					else if ((orderRobots == std::partial_ordering::greater || orderRobots == std::partial_ordering::equivalent)
						&& (orderResources == std::partial_ordering::greater || orderResources == std::partial_ordering::equivalent)
						&& timeSpent <= oTime)
					{
						oTime = timeSpent;
						oRobots = robots;
						oResources = resources;
						skipInsert = true;
						break;
					}
				}
			}

			if (prune)
			{
				continue;
			}
			else if (!skipInsert)
			{
				bestStates.emplace_back(robots, resources, timeSpent);
			}

			if (timeSpent == runTime)
			{
				if (resources[Resource::Geode] > localScore)
				{
					localScore = resources[Resource::Geode];
				}
			}
			else
			{
				//If we can build a Geode bot, we should always build it
				if (factory.CanBuild(Resource::Geode, resources))
				{
					auto nextResources = factory.BuildRobot(Resource::Geode, resources);
					fillQueue.push_front(std::make_tuple(robots, nextResources, Resource::Geode, timeSpent + 1));
				}
				else
				{
					for (auto buildBot : buildOptions)
					{
						if (!buildBot.has_value())
						{
							fillQueue.push_front(std::make_tuple(robots, resources, buildBot, timeSpent + 1));
						}
						else if (factory.CanBuild(buildBot.value(), resources))
						{
							if (factory.ShouldBuild(buildBot.value(), robots[buildBot.value()], resources[buildBot.value()], runTime - timeSpent))
							{
								auto nextResources = factory.BuildRobot(buildBot.value(), resources);

								fillQueue.push_front(std::make_tuple(robots, nextResources, buildBot, timeSpent + 1));
							}
						}
					}
				}
			}
		}

		return localScore;
	};

	std::vector<std::future<std::uint32_t>> scoreFutures;

	for (auto& factory : factories)
	{
		scoreFutures.push_back(std::async(std::launch::async, [&]() {return runBlueprint(factory, 24); }));
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

	//Part 2
	scoreFutures.clear();
	for (std::size_t i{ 0 }; i < 3 && i < factories.size(); i++)
	{
		scoreFutures.push_back(std::async(std::launch::async, [&, &factory = factories[i]] {return runBlueprint(factory, 32); }));
	}

	bestScore.clear();
	for (auto& f : scoreFutures)
	{
		bestScore.push_back(f.get());
	}

	score = 1;
	for (auto s : bestScore)
	{
		score *= s;
		std::cout << s << " ";
	}
	std::cout << score << "\n";
}
