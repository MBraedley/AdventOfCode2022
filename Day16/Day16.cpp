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
	std::string line;

	std::regex valveName("([A-Z]{2})");
	std::regex flowRate("flow rate=(\\d+)");
	std::smatch m;

	std::size_t valveCount{ 0 };

	while (std::getline(inStrm, line))
	{
		input.push_back(line);
		std::regex_search(line, m, valveName);
		std::string name = m[1];
		std::regex_search(line, m, flowRate);
		std::int64_t rate = std::stoull(m[1]);
		valves[name] = std::make_shared<Valve>( name, FlowRate( rate ) );

		if( rate > 0 )
		{
			valveCount++;
		}
	}

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

	using PathScore = std::tuple<std::shared_ptr<Valve>, TimeLeft, std::unordered_set<std::string>, std::unordered_set<std::string>, bool, Score, std::string>;

	auto pathComp = []( const PathScore& l, const PathScore& r ) -> bool
	{
		auto& [lValve, lTimeLeft, lOpenedValves, lVistedValves, lOpenValve, lScore, lPathSoFar] = l;
		auto& [rValve, rTimeLeft, rOpenedValves, rVistedValves, rOpenValve, rScore, rPathSoFar] = r;

		assert( (lOpenedValves.contains( lValve->GetName() ) && lOpenValve) == false );
		assert( (rOpenedValves.contains( rValve->GetName() ) && rOpenValve) == false );
		
		Score lNewScore = lScore;
		Score rNewScore = rScore;
		
		if( !lValve->IsOpened() )
		{
			if( lOpenValve )
			{
				lNewScore += lValve->GetScore( lTimeLeft );
			}
			else
			{
				lNewScore += lValve->GetScore( lTimeLeft - onePeriod );
			}
		}

		if( !rValve->IsOpened() )
		{
			if( rOpenValve )
			{
				rNewScore += rValve->GetScore( rTimeLeft );
			}
			else
			{
				rNewScore += rValve->GetScore( rTimeLeft - onePeriod );
			}
		}

		if( lNewScore == rNewScore )
		{
			return lTimeLeft < rTimeLeft;
		}

		return lNewScore < rNewScore;
	};

	std::priority_queue<PathScore, std::vector<PathScore>, decltype(pathComp)> searchQueue(pathComp);
	searchQueue.push( { valves["AA"], TimeLeft( 31 ), {}, {}, false, Score( 0 ), "" } );
	Score bestScore{ 0 };
	std::string bestPath;

	while( !searchQueue.empty() )
	{
		PathScore path = searchQueue.top();
		searchQueue.pop();
		auto& [valve, timeLeft, openedValves, vistedValves, openValve, score, pathSoFar] = path;
		if( score > bestScore )
		{
			bestScore = score;
			bestPath = pathSoFar;
		}

		assert( !vistedValves.contains( valve->GetName() ) || openValve );
		vistedValves.insert( valve->GetName() );
		pathSoFar += valve->GetName() + "->";

		if( openedValves.size() == valveCount )
		{
			std::priority_queue<PathScore, std::vector<PathScore>, decltype(pathComp)> emptyQueue( pathComp );
			std::swap( searchQueue, emptyQueue );
		}

		if( timeLeft > TimeLeft( 0 ) && openedValves.size() < valveCount )
		{
			timeLeft--;

			if( valve->GetFlowRate().get() > 0 )
			{
				if( openValve )
				{
					assert( !openedValves.contains( valve->GetName() ) );
					valve->OpenValve();
					openedValves.insert( valve->GetName() );
					score += valve->GetScore( timeLeft );
					pathSoFar += "open->";
					vistedValves.clear();
				}
				else if( !openedValves.contains( valve->GetName() ) )
				{
					searchQueue.push( { valve, timeLeft, openedValves, vistedValves, true, score, pathSoFar } );
				}
			}

			for( std::shared_ptr<Valve> connection : valve->GetConnections() )
			{
				if( !vistedValves.contains( connection->GetName() ) )
				{
					searchQueue.push( { connection, timeLeft, openedValves, vistedValves, false, score, pathSoFar } );
				}
			}
		}
		//std::cout << pathSoFar << "\n";
	}

	std::cout << bestPath << "\n" << bestScore.get() << "\n";
}
