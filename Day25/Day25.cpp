#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cassert>

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::int64_t> input;
	std::ifstream inStrm(inputFile);

	std::string line;
	while (std::getline(inStrm, line))
	{
		std::int64_t snafu{ 0 };
		std::reverse(line.begin(), line.end());
		for (std::int64_t p = 0; p < line.size(); p++)
		{
			switch (line[p])
			{
			case '2':
				snafu += 2 * std::pow(5ll, p);
				break;
			case '1':
				snafu += 1 * std::pow(5ll, p);
				break;
			case '0':
				break;
			case '-':
				snafu -= 1 * std::pow(5ll, p);
				break;
			case '=':
				snafu -= 2 * std::pow(5ll, p);
				break;
			default:
				assert(false);
				break;
			}
		}
		input.push_back(snafu);
	}

	std::int64_t sum{ 0 };
	for (auto snafu : input)
	{
		sum += snafu;
	}

	std::cout << sum << "\n";

	std::string output;
	while (sum > 0)
	{
		switch (sum % 5)
		{
		case 0:
			output.push_back('0');
			break;
		case 1:
			output.push_back('1');
			break;
		case 2:
			output.push_back('2');
			break;
		case 3:
			output.push_back('=');
			sum += 5;
			break;
		case 4:
			output.push_back('-');
			sum += 5;
			break;
		default:
			break;
		}
		sum /= 5;
	}

	std::reverse(output.begin(), output.end());
	std::cout << output << "\n";
}
