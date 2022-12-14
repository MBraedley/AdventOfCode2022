#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <variant>
#include <memory>
#include <stack>

struct Node
{
	using NodeValue = std::variant<std::shared_ptr<Node>, std::int32_t>;
	std::vector<NodeValue> subNodes;
};

std::shared_ptr<Node> ParsePacket(std::string input)
{
	std::stack<std::shared_ptr<Node>> parseStack;
	std::string numUnderParse = "";

	for (const auto& c : input)
	{
		if (c == '[')
		{
			auto node = std::make_shared<Node>();
			if (!parseStack.empty())
			{
				parseStack.top()->subNodes.push_back(node);
			}
			parseStack.push(node);
		}
		else if (c == ']')
		{
			auto node = parseStack.top();
			if (!numUnderParse.empty())
			{
				node->subNodes.push_back(std::stoi(numUnderParse));
				numUnderParse.clear();
			}

			parseStack.pop();
			if (parseStack.empty())
			{
				return node;
			}
		}
		else if (c == ',')
		{
			if (!numUnderParse.empty())
			{
				parseStack.top()->subNodes.push_back(std::stoi(numUnderParse));
				numUnderParse.clear();
			}
		}
		else
		{
			numUnderParse.push_back(c);
		}
	}

	throw std::exception("Unbalanced []");
}

void PrintNode(std::shared_ptr<Node> node);
void PrintNodeValue(const Node::NodeValue& nodeVal)
{
	if (std::holds_alternative<std::int32_t>(nodeVal))
	{
		std::cout << std::get<std::int32_t>(nodeVal);
	}
	else
	{
		PrintNode(std::get<std::shared_ptr<Node>>(nodeVal));
	}
}

void PrintNode(std::shared_ptr<Node> node)
{
	std::cout << "[";

	for (std::size_t i{ 0 }; i < node->subNodes.size(); i++)
	{
		if (i != 0)
		{
			std::cout << ",";
		}
		PrintNodeValue(node->subNodes[i]);
	}

	std::cout << "]";
}

std::strong_ordering operator<=>(std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs);
std::strong_ordering operator<=>(Node::NodeValue lhs, Node::NodeValue rhs)
{
	if (std::holds_alternative<std::int32_t>(lhs) && std::holds_alternative<std::int32_t>(rhs))
	{
		return std::get<std::int32_t>(lhs) <=> std::get<std::int32_t>(rhs);
	}
	else if (std::holds_alternative<std::shared_ptr<Node>>(lhs) && std::holds_alternative<std::shared_ptr<Node>>(rhs))
	{
		return std::get<std::shared_ptr<Node>>(lhs) <=> std::get<std::shared_ptr<Node>>(rhs);
	}
	else if (std::holds_alternative<std::int32_t>(lhs) && std::holds_alternative<std::shared_ptr<Node>>(rhs))
	{
		auto lhsNode = std::make_shared<Node>();
		lhsNode->subNodes.push_back(lhs);
		return lhsNode <=> std::get<std::shared_ptr<Node>>(rhs);
	}
	else if (std::holds_alternative<std::shared_ptr<Node>>(lhs) && std::holds_alternative<std::int32_t>(rhs))
	{
		auto rhsNode = std::make_shared<Node>();
		rhsNode->subNodes.push_back(rhs);
		return std::get<std::shared_ptr<Node>>(lhs) <=> rhsNode;
	}

	throw std::exception("Bad variant");
}

std::strong_ordering operator<=>(std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs)
{
	//std::cout << "Comparing ";
	//PrintNode(lhs);
	//std::cout << " and ";
	//PrintNode(rhs);
	//std::cout << "\n";

	if (lhs->subNodes.empty() && rhs->subNodes.empty())
	{
		return std::strong_ordering::equal;
	}
	else if (lhs->subNodes.empty())
	{
		return std::strong_ordering::less;
	}
	else if (rhs->subNodes.empty())
	{
		return std::strong_ordering::greater;
	}

	for (std::size_t i{ 0 }; i < std::min(lhs->subNodes.size(), rhs->subNodes.size()); i++)
	{
		auto order = lhs->subNodes[i] <=> rhs->subNodes[i];
		if (order != std::strong_ordering::equal)
		{
			return order;
		}
	}

	return lhs->subNodes.size() <=> rhs->subNodes.size();
}

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

	std::size_t index = 1;
	std::size_t correctIndices = 0;

	std::vector<std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>>> parsedInput;
	for (auto& [left, right] : input)
	{
		auto[pLeft, pRight] = parsedInput.emplace_back(ParsePacket(left), ParsePacket(right));
		auto order = pLeft <=> pRight;
		if (order == std::strong_ordering::equal)
		{
			//std::cout << "lhs == rhs";
		}
		else if (order == std::strong_ordering::less)
		{
			//std::cout << "lhs < rhs";
			correctIndices += index;
		}
		else if (order == std::strong_ordering::greater)
		{
			//std::cout << "lhs > rhs";
		}
		else
		{
			throw std::exception("bad ordering");
		}
		std::cout << "\n\n";
		index++;
	}
	std::cout << "Part 1:" << correctIndices << "\n";

	//Part 2
	std::vector<std::shared_ptr<Node>> allPackets;
	allPackets.reserve(parsedInput.size() * 2 + 2);

	for (auto& [left, right] : parsedInput)
	{
		allPackets.push_back(left);
		allPackets.push_back(right);
	}

	auto div2 = ParsePacket("[[2]]");
	auto div6 = ParsePacket("[[6]]");
	allPackets.push_back(div2);
	allPackets.push_back(div6);

	std::ranges::sort(allPackets, [](std::shared_ptr<Node> lhs, std::shared_ptr<Node> rhs) -> bool
		{
			return (lhs <=> rhs) == std::strong_ordering::less;
		});

	std::size_t index2 = 0;
	std::size_t index6 = 0;

	for (std::size_t i{ 0 }; i < allPackets.size(); i++)
	{
		if ((div2 <=> allPackets[i]) == std::strong_ordering::equal)
		{
			index2 = i + 1;
		}
		if ((div6 <=> allPackets[i]) == std::strong_ordering::equal)
		{
			index6 = i + 1;
		}
	}

	std::cout << "Part 2: " << index2 * index6 << "\n";
}
