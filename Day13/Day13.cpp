#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <variant>
#include <memory>

class Packet
{
public:
private:
	struct Node
	{
		using NodeValue = std::variant<std::shared_ptr<Node>, std::int32_t, std::monostate>;
		std::vector<NodeValue> subNodes;
	};


};

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::pair<std::string, std::string>> input;
	std::ifstream inStrm(inputFile);

	std::string line;
	while (std::getline(inStrm, line))
	{
		std::string in1 = line;
		std::getline(inStrm, line);
		input.emplace_back(in1, line);
		std::getline(inStrm, line);	//empty line
	}

	for (auto& [left, right] : input)
	{

	}
}
