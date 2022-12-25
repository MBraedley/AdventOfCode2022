#include "NamedType/named_type.hpp"

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
#include <queue>
#include <unordered_set>
#include <map>
#include <set>
#include <sstream>
#include <chrono>

using FlowRate = fluent::NamedType<std::int64_t, struct _FlowRate, fluent::Arithmetic>;
using TimeLeft = fluent::NamedType<std::int64_t, struct _TimeLeft, fluent::Arithmetic>;
using Score = fluent::NamedType<std::int64_t, struct _Score, fluent::Arithmetic>;

constexpr TimeLeft onePeriod{ 1 };

class Valve
{
public:
	Valve(const std::string& name, FlowRate flowRate) :
		m_Name(name),
		m_FlowRate(flowRate)
	{}

	void AddConnection(std::shared_ptr<Valve> tunnelTo)
	{
		m_Connections.push_back(tunnelTo);
	}

	void OpenValve()
	{
		m_IsOpened = true;
	}

	bool IsOpened() const { return m_IsOpened; }

	FlowRate GetFlowRate() const { return m_FlowRate; }

	Score GetScore( TimeLeft timeLeft) const
	{
		return Score( m_FlowRate.get() * timeLeft.get() );
	}

	const std::vector<std::shared_ptr<Valve>>& GetConnections() { return m_Connections; }

	const std::string& GetName() const { return m_Name; }

	std::strong_ordering operator<=>(const Valve& other)
	{
		if (this->m_Name == other.m_Name)
		{
			return std::strong_ordering::equal;
		}
		if (this->GetFlowRate() == other.GetFlowRate())
		{
			return std::strong_ordering::equivalent;
		}
		return this->GetFlowRate() <=> other.GetFlowRate();
	}

private:
	std::string m_Name;
	FlowRate m_FlowRate;
	bool m_IsOpened{ false };
	std::vector<std::shared_ptr<Valve>> m_Connections;
};

std::weak_ordering operator<=>( const std::shared_ptr<Valve>& lhs, const std::shared_ptr<Valve>& rhs )
{
	return *lhs <=> *rhs;
}

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::string> input;
	std::ifstream inStrm(inputFile);

	std::unordered_map<std::string, std::shared_ptr<Valve>> valves;
	std::unordered_map<std::string, std::shared_ptr<Valve>> workingValves;
	std::string line;

	std::regex valveName("([A-Z]{2})");
	std::regex flowRate("flow rate=(\\d+)");
	std::smatch m;

	//std::unordered_multiset<FlowRate> unopenedValves;
	auto start = std::chrono::system_clock::now();

	//create all valves
	while (std::getline(inStrm, line))
	{
		input.push_back(line);
		std::regex_search(line, m, valveName);
		std::string name = m[1];
		std::regex_search(line, m, flowRate);
		FlowRate rate(std::stoull(m[1]));
		valves[name] = std::make_shared<Valve>( name, rate );

		if( rate.get() > 0)
		{
			workingValves[name] = valves[name];
		}
	}

	workingValves["AA"] = valves["AA"];	//Add the starting valve

	//create connections
	for (auto valveLine : input)
	{
		std::regex_search(valveLine, m, valveName);
		std::string name = m[1];
		valveLine = m.suffix();

		while (std::regex_search(valveLine, m, valveName))
		{
			assert(valves.contains(m[1]));
			valves[name]->AddConnection(valves[m[1]]);
			valveLine = m.suffix();
		}
	}

	//Generate pairwise distances
	std::unordered_multimap<std::string, std::tuple<std::string, TimeLeft>> distances;
	for (const auto& [name, _] : workingValves)
	{
		std::unordered_map<std::string, TimeLeft> visited;
		std::queue<std::pair<std::string, TimeLeft>> nextValves;
		nextValves.emplace( name, 0ll );
		while (!nextValves.empty())
		{
			auto& [testedName, travelTime] = nextValves.front();

			if (!visited.contains(testedName) || visited[testedName] > travelTime)
			{
				visited[testedName] = travelTime;
				for (auto& neighbour : valves[testedName]->GetConnections())
				{
					nextValves.emplace(neighbour->GetName(), travelTime + onePeriod);
				}
			}

			nextValves.pop();
		}

		for (const auto& [otherName, _] : workingValves)
		{
			if (name != otherName)
			{
				assert(visited.contains(otherName));
				distances.emplace(name, std::make_tuple(otherName, visited[otherName] + onePeriod));
			}
		}
	}

	auto preprocessing = std::chrono::system_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(preprocessing - start) << "\n";

	using Path = std::tuple<std::shared_ptr<Valve>, std::unordered_set<std::string>, TimeLeft, Score>;

	std::queue<Path> searchQueue;
	searchQueue.push({ valves["AA"], {}, TimeLeft(30), Score(0) });
	Score bestScore{ 0 };

	while (!searchQueue.empty())
	{
		auto [val, visited, timeLeft, score] = searchQueue.front();
		searchQueue.pop();

		visited.emplace(val->GetName());
		score += val->GetScore(timeLeft);

		if (score > bestScore)
		{
			bestScore = score;
		}

		auto otherValves = distances.equal_range(val->GetName());
		for (auto iter = otherValves.first; iter != otherValves.second; iter++)
		{
			std::string otherName = std::get<std::string>(iter->second);
			TimeLeft distTo = std::get<TimeLeft>(iter->second);
			
			if (!visited.contains(otherName) && distTo < timeLeft)
			{
				searchQueue.emplace(valves[otherName], visited, timeLeft - distTo, score);
			}
		}
	}
	
	auto part1 = std::chrono::system_clock::now();

	std::cout << bestScore.get() << "\n";
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(part1 - preprocessing) << "\n";

	//Part 2
	std::queue<std::pair<Path, Path>> search2Queue;
	search2Queue.push(std::make_pair<Path, Path>({ valves["AA"], {}, TimeLeft(26), Score(0) }, { valves["AA"], {}, TimeLeft(26), Score(0) }));
	
	std::map<std::string, Score> stateScores;

	auto stateToString = [](const std::unordered_set<std::string>& state) -> std::string
	{
		std::set<std::string> orderedState(state.begin(), state.end());
		std::stringstream sstrm;
		for (auto& s : orderedState)
		{
			sstrm << s << ",";
		}

		return sstrm.str();
	};

	while (!search2Queue.empty())
	{
		auto [p1, p2] = search2Queue.front();
		search2Queue.pop();

		auto [val1, visited1, timeLeft1, score1] = p1;
		auto [val2, visited2, timeLeft2, score2] = p2;

		score1 += val1->GetScore(timeLeft1);
		score2 += val2->GetScore(timeLeft2);

		std::unordered_set<std::string> state;
		state.insert(visited1.begin(), visited1.end());
		state.insert(visited2.begin(), visited2.end());
		state.insert(val1->GetName());
		state.insert(val2->GetName());

		std::string stateStr = stateToString(state);
		if (!stateScores.contains(stateStr) || stateScores[stateStr] < score1 + score2)
		{
			stateScores[stateStr] = score1 + score2;
		}
		else
		{
			continue;	//No point continuing on this branch, it won't produce a better score
		}

		if (!visited1.contains(val1->GetName()))
		{
			visited1.emplace(val1->GetName());

			auto otherValves1 = distances.equal_range(val1->GetName());
			for (auto iter = otherValves1.first; iter != otherValves1.second; iter++)
			{
				std::string otherName = std::get<std::string>(iter->second);
				TimeLeft distTo = std::get<TimeLeft>(iter->second);

				if (!visited1.contains(otherName) && !visited2.contains(otherName) && otherName != val2->GetName() && distTo < timeLeft1)
				{
					search2Queue.emplace(std::make_tuple(valves[otherName], visited1, timeLeft1 - distTo, score1), p2);
				}
			}
		}

		if (!visited2.contains(val2->GetName()))
		{
			visited2.emplace(val2->GetName());
			auto otherValves2 = distances.equal_range(val2->GetName());
			for (auto iter = otherValves2.first; iter != otherValves2.second; iter++)
			{
				std::string otherName = std::get<std::string>(iter->second);
				TimeLeft distTo = std::get<TimeLeft>(iter->second);

				if (!visited1.contains(otherName) && !visited2.contains(otherName) && otherName != val1->GetName() && distTo < timeLeft2)
				{
					search2Queue.emplace(p1, std::make_tuple(valves[otherName], visited2, timeLeft2 - distTo, score2));
				}
			}
		}
	}

	bestScore.get() = 0;
	for (auto& [state, score] : stateScores)
	{
		if (score > bestScore)
		{
			bestScore = score;
		}
	}

	std::cout << bestScore.get() << "\n";
}
