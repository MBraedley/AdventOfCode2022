#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <deque>
#include <chrono>
#include <regex>

class Monkey
{
public:
	Monkey() = delete;
	
	Monkey(
		const std::deque<std::uint64_t> items,
		std::function<std::uint64_t(std::uint64_t)> op,
		std::function<unsigned int(std::uint64_t)> test,
		std::function<void(unsigned int, std::uint64_t)> callback ) :
		m_Items( items ),
		m_Operation(op),
		m_Test(test),
		m_ThrowCallback(callback)
	{}
	
	~Monkey() = default;

	void CatchItem(std::uint64_t item)
	{
		m_Items.push_back(item);
	}

	std::size_t InspectItems1()
	{
		std::size_t ret = m_Items.size();

		while (!m_Items.empty())
		{
			std::uint64_t item = m_Items.front();
			m_Items.pop_front();

			item = m_Operation(item) / 3;
			unsigned int catcher = m_Test(item);
			m_ThrowCallback(catcher, item);
		}

		return ret;
	}

	std::size_t InspectItems2()
	{
		std::size_t ret = m_Items.size();

		while (!m_Items.empty())
		{
			std::uint64_t item = m_Items.front();
			m_Items.pop_front();

			item = m_Operation(item);
			unsigned int catcher = m_Test(item);
			m_ThrowCallback(catcher, item);
		}

		return ret;
	}

private:
	std::deque<std::uint64_t> m_Items;
	std::function<std::uint64_t(std::uint64_t)> m_Operation;
	std::function<unsigned int(std::uint64_t)> m_Test;
	std::function<void(unsigned int, std::uint64_t)> m_ThrowCallback;
};

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);

	auto startTime = std::chrono::system_clock::now();

	std::vector<std::unique_ptr<Monkey>> monkeys;
	std::string line;

	std::regex findNum("(\\d+)");
	std::regex getOp("(old) (\\+|\\*) (old|\\d+)");
	std::smatch m;

	std::function<void(unsigned int, std::uint64_t)> callback = [&](unsigned int monkey, std::uint64_t item)
	{
		monkeys[monkey]->CatchItem(item);
	};

	std::uint64_t worryCap{ 1 };

	while (std::getline(inStrm, line))
	{
		//monkey ID, don't care about it
		std::getline(inStrm, line);
		std::deque<std::uint64_t> startingItems;
		while (std::regex_search(line, m, findNum))
		{
			startingItems.push_back(std::stoi(m[1]));
			line = m.suffix();
		}

		std::getline(inStrm, line);
		std::regex_search(line, m, getOp);
		std::function<std::uint64_t(std::uint64_t)> op;
		if (m[3] == "old")
		{
			if (m[2] == "+")
			{
				op = [&](std::uint64_t old) { return (old + old) % worryCap; };
			}
			else
			{
				op = [&](std::uint64_t old) { return (old * old) % worryCap; };
			}
		}
		else
		{
			std::uint64_t val = std::stoull(m[3]);

			if (m[2] == "+")
			{
				op = [&, val = val](std::uint64_t old) { return (old + val) % worryCap; };
			}
			else
			{
				op = [&, val = val](std::uint64_t old) { return (old * val) % worryCap; };
			}
		}

		std::getline(inStrm, line);
		std::regex_search(line, m, findNum);
		std::uint64_t divisor = std::stoull(m[1]);
		worryCap *= divisor;

		std::getline(inStrm, line);
		std::regex_search(line, m, findNum);
		unsigned int ifTrueMonkey = std::stoul(m[1]);

		std::getline(inStrm, line);
		std::regex_search(line, m, findNum);
		unsigned int ifFalseMonkey = std::stoul(m[1]);

		std::function<unsigned int(std::uint64_t)> test = [=](std::uint64_t item) -> unsigned int
		{
			if (item % divisor == 0)
			{
				return ifTrueMonkey;
			}
			else
			{
				return ifFalseMonkey;
			}
		};

		std::getline(inStrm, line);	//empty line;

		monkeys.emplace_back(std::make_unique<Monkey>(startingItems, op, test, callback));
	}

	auto endRead = std::chrono::system_clock::now();

	constexpr const int part = 2;

	std::vector<std::size_t> itemsInspected(monkeys.size(), 0ull);

	if constexpr (part == 1)
	{
		for (int i = 0; i < 20; i++)
		{
			for (std::size_t m{ 0 }; m < monkeys.size(); m++)
			{
				itemsInspected[m] += monkeys[m]->InspectItems1();
			}
		}
	}
	else
	{
		for (int i = 0; i < 10000; i++)
		{
			for (std::size_t m{ 0 }; m < monkeys.size(); m++)
			{
				itemsInspected[m] += monkeys[m]->InspectItems2();
			}
		}
	}

	std::sort(itemsInspected.begin(), itemsInspected.end(), std::greater<std::size_t>());

	auto endCalc = std::chrono::system_clock::now();

	std::cout << itemsInspected[0] * itemsInspected[1] << "\n";

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endRead - startTime) << ", "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(endCalc - endRead) << "\n";
}
