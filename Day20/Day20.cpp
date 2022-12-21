#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <deque>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::ifstream inStrm(inputFile);
	std::vector<std::int64_t> mixOrder;
	std::deque<std::int64_t> inputPt1;
	std::deque<std::int64_t> inputPt2;

	constexpr std::int64_t decryptKey = 811589153;

	std::int64_t val;
	while (inStrm>>val)
	{
		mixOrder.emplace_back(val);
		inputPt1.emplace_back(val);
		inputPt2.emplace_back(val * decryptKey);
	}

	std::int64_t codeLength = mixOrder.size();

	auto MixList = [&](std::deque<std::int64_t>& input)
	{
		//for (auto i : input)
		//{
		//	std::cout << i << ", ";
		//}
		//std::cout << "\n";

		for (std::int64_t val : mixOrder)
		{
			while (input.front() != val)
			{
				input.push_back(input.front());
				input.pop_front();
			}

			auto iter = input.begin();

			std::int64_t endIndex = val % (codeLength - 1);
			if (endIndex < 0)
			{
				endIndex += codeLength - 1;
			}

			input.erase(iter);
			input.emplace(input.begin() + endIndex, val);

			//for (auto i : input)
			//{
			//	std::cout << i << ", ";
			//}
			//std::cout << "\n";
		}
	};

	auto PrintOutput = [codeLength](std::deque<std::int64_t>& input)
	{
		while (input.front() != 0)
		{
			input.push_back(input.front());
			input.pop_front();
		}

		std::cout << *(input.begin() + 1000 % codeLength) << " " << * (input.begin() + 2000 % codeLength) << " " << *(input.begin() + 3000 % codeLength) << " = ";
		std::cout << *(input.begin() + 1000 % codeLength) + * (input.begin() + 2000 % codeLength) + *(input.begin() + 3000 % codeLength) << "\n";
	};

	MixList(inputPt1);
	PrintOutput(inputPt1);

	//Part 2
	std::transform(mixOrder.begin(), mixOrder.end(), mixOrder.begin(), [](std::int64_t val) {return val * decryptKey; });
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
