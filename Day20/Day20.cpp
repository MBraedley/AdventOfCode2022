#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <deque>

int main()
{
	using EncryptedCoordList = std::deque<std::pair<std::int64_t, std::size_t>>;

	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);
	EncryptedCoordList inputPt1;
	EncryptedCoordList inputPt2;

	constexpr std::int64_t decryptKey = 811589153;
	std::int64_t codeLength{ 0 };

	std::int64_t val;
	while (inStrm>>val)
	{
		inputPt1.emplace_back(val, codeLength);
		inputPt2.emplace_back(val * decryptKey, codeLength);
		codeLength++;
	}


	auto MixList = [&](EncryptedCoordList& input)
	{
		//for (auto i : input)
		//{
		//	std::cout << i << ", ";
		//}
		//std::cout << "\n";

		for (std::size_t i{0}; i < codeLength; i++)
		{
			while (input.front().second != i)
			{
				input.push_back(input.front());
				input.pop_front();
			}

			auto iter = input.begin();
			val = iter->first;

			std::int64_t endIndex = val % (codeLength - 1);
			if (endIndex < 0)
			{
				endIndex += codeLength - 1;
			}

			input.erase(iter);
			input.emplace(input.begin() + endIndex, val, i);

			//for (auto i : input)
			//{
			//	std::cout << i << ", ";
			//}
			//std::cout << "\n";
		}
	};

	auto PrintOutput = [codeLength](EncryptedCoordList& input)
	{
		while (input.front().first != 0)
		{
			input.push_back(input.front());
			input.pop_front();
		}

		std::cout << (input.begin() + 1000 % codeLength)->first << " " << (input.begin() + 2000 % codeLength)->first << " " << (input.begin() + 3000 % codeLength)->first << " = ";
		std::cout << (input.begin() + 1000 % codeLength)->first + (input.begin() + 2000 % codeLength)->first + (input.begin() + 3000 % codeLength)->first << "\n";
	};

	MixList(inputPt1);
	PrintOutput(inputPt1);

	//Part 2
	for (int i = 0; i < 10; i++)
	{
		MixList(inputPt2);
		//for (auto i : inputPt2)
		//{
		//	std::cout << i << ", ";
		//}
		//std::cout << "\n";
	}
	PrintOutput(inputPt2);
}
