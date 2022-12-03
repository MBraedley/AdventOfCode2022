#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <utility>
#include <set>
#include <deque>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::pair<std::string, std::string>> rucksacks;
	std::ifstream inStrm(inputFile);

	std::string line;
	std::uint64_t calCount = 0;
	while (std::getline(inStrm, line))
	{
		std::string compA = line.substr(0, line.length() / 2);
		std::string compB = line.substr(line.length() / 2);
		rucksacks.emplace_back(compA, compB);
	}

	auto getVal = [](const char& item) -> std::uint64_t
	{
		if (item >= 'a' && item <= 'z')
		{
			return std::uint64_t(item) - 'a' + 1;
		}
		else if (item >= 'A' && item <= 'Z')
		{
			return std::uint64_t(item) - 'A' + 27;
		}
		else
		{
			throw std::exception("something went wrong");
		}
	};

	std::uint64_t priorities = 0;
	for (const auto& [compA, compB] : rucksacks)
	{
		std::set<char> contentsA(compA.begin(), compA.end());
		for (const char& c : contentsA)
		{
			if (compB.find(c) != std::string::npos)
			{
				auto p = getVal(c);
				priorities += p;
			}
		}
	}

	std::cout << priorities << "\n";

	//Part 2
	if (rucksacks.size() % 3 != 0)
	{
		throw std::exception("wrong number of rucksacks");
	}

	priorities = 0;

	std::set<char> resolvedGroups;
	std::deque<std::set<char>> unresolvedGroups;

	for (auto i = 0ll; i < rucksacks.size() / 3; i++)
	{
		auto sack1 = rucksacks[i].first + rucksacks[i].second;
		std::set<char> elf1(sack1.begin(), sack1.end());
		auto sack2 = rucksacks[i+1].first + rucksacks[i+1].second;
		std::set<char> elf2(sack2.begin(), sack2.end());
		auto sack3 = rucksacks[i+2].first + rucksacks[i+2].second;
		std::set<char> elf3(sack3.begin(), sack3.end());

		std::string inter1;
		std::set_intersection(elf1.begin(), elf1.end(), elf2.begin(), elf2.end(), std::back_inserter(inter1));
		std::string inter2;
		std::set_intersection(inter1.begin(), inter1.end(), elf3.begin(), elf3.end(), std::back_inserter(inter2));

		if (inter2.length() == 1)
		{
			resolvedGroups.insert(inter2[0]);
		}
		else
		{
			unresolvedGroups.emplace_back(inter2.begin(), inter2.end());
		}
	}

	while (!unresolvedGroups.empty())
	{
		auto group = unresolvedGroups.front();
		unresolvedGroups.pop_front();

		for (const char& c : group)
		{
			if (resolvedGroups.contains(c))
			{
				group.erase(c);
			}
		}

		if (group.size() == 1)
		{
			resolvedGroups.insert(*group.begin());
		}
		else
		{
			if (group.size() == 0)
			{
				throw std::exception("");
			}

			unresolvedGroups.push_back(group);
		}
	}

	for (char c : resolvedGroups)
	{
		priorities += getVal(c);
	}
	std::cout << priorities << "\n";
}
