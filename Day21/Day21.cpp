#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <variant>
#include <unordered_map>
#include <regex>

struct Monkey
{
	using Branch = std::variant<std::monostate, std::int64_t, std::tuple<std::pair<std::shared_ptr<Monkey>, std::shared_ptr<Monkey>>, std::function<std::int64_t(std::shared_ptr<Monkey>, std::shared_ptr<Monkey>)>>>;

	Branch m_SubMonkeys{};
	std::shared_ptr<Monkey> m_Parent{ nullptr };

	std::int64_t GetNum()
	{
		if (std::holds_alternative<std::int64_t>(m_SubMonkeys))
		{
			auto val = std::get<std::int64_t>(m_SubMonkeys);
			std::cout << val;
			return val;
		}
		else
		{
			auto [monkeys, op] = std::get<std::tuple<std::pair<std::shared_ptr<Monkey>, std::shared_ptr<Monkey>>, std::function<std::int64_t(std::shared_ptr<Monkey>, std::shared_ptr<Monkey>)>>>(m_SubMonkeys);
			return op(monkeys.first, monkeys.second);
		}
	}
};


int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::string> input;
	std::ifstream inStrm(inputFile);

	std::unordered_map<std::string, std::shared_ptr<Monkey>> tree;

	std::string line;
	std::uint64_t calCount = 0;
	while (std::getline(inStrm, line))
	{
		input.push_back(line);
		tree.emplace(line.substr(0, line.find_first_of(':')), std::make_shared<Monkey>());
	}

	std::regex withNumber("([a-z]{4}): (\\d+)");
	std::regex withMonkeys("([a-z]{4}): ([a-z]{4}) (.) ([a-z]{4})");
	std::smatch m;

	auto addOp = [](std::shared_ptr<Monkey> lhs, std::shared_ptr<Monkey> rhs) -> std::int64_t
	{
		std::cout << "(";
		auto l = lhs->GetNum();
		std::cout << "+";
		auto r = rhs->GetNum();
		std::cout << ")";
		return l + r;
	};

	auto subOp = [](std::shared_ptr<Monkey> lhs, std::shared_ptr<Monkey> rhs) -> std::int64_t
	{
		std::cout << "(";
		auto l = lhs->GetNum();
		std::cout << "-";
		auto r = rhs->GetNum();
		std::cout << ")";
		return l - r;
	};

	auto mulOp = [](std::shared_ptr<Monkey> lhs, std::shared_ptr<Monkey> rhs) -> std::int64_t
	{
		std::cout << "(";
		auto l = lhs->GetNum();
		std::cout << "*";
		auto r = rhs->GetNum();
		std::cout << ")";
		return l * r;
	};

	auto divOp = [](std::shared_ptr<Monkey> lhs, std::shared_ptr<Monkey> rhs) -> std::int64_t
	{
		std::cout << "(";
		auto l = lhs->GetNum();
		std::cout << "/";
		auto r = rhs->GetNum();
		std::cout << ")";
		return l / r;
	};

	for (auto& monkeyDef : input)
	{
		if (std::regex_match(monkeyDef, m, withNumber))
		{
			tree[m[1]]->m_SubMonkeys = std::stoi(m[2]);
		}
		else if (std::regex_match(monkeyDef, m, withMonkeys))
		{
			switch (m[3].str()[0])
			{
			case '+':
				tree[m[1]]->m_SubMonkeys = std::make_tuple(std::make_pair(tree[m[2]], tree[m[4]]), addOp);
				break;
			case '-':
				tree[m[1]]->m_SubMonkeys = std::make_tuple(std::make_pair(tree[m[2]], tree[m[4]]), subOp);
				break;
			case '*':
				tree[m[1]]->m_SubMonkeys = std::make_tuple(std::make_pair(tree[m[2]], tree[m[4]]), mulOp);
				break;
			case '/':
				tree[m[1]]->m_SubMonkeys = std::make_tuple(std::make_pair(tree[m[2]], tree[m[4]]), divOp);
				break;
			default:
				throw std::exception("bad op");
				break;
			}

			tree[m[2]]->m_Parent = tree[m[1]];
			tree[m[4]]->m_Parent = tree[m[1]];
		}
		else
		{
			throw std::exception("No Match");
		}
	}

	std::cout << "\n" << tree["root"]->GetNum() << "\n";

	//Part 2
	std::cout << "\n\n";

	auto [pairMonkeys, op] = std::get<std::tuple<std::pair<std::shared_ptr<Monkey>, std::shared_ptr<Monkey>>, std::function<std::int64_t(std::shared_ptr<Monkey>, std::shared_ptr<Monkey>)>>>(tree["root"]->m_SubMonkeys);

	std::shared_ptr<Monkey> human = tree["humn"];
	
	human->m_SubMonkeys = std::make_tuple(std::make_pair<std::shared_ptr<Monkey>, std::shared_ptr<Monkey>>(nullptr, nullptr), [](std::shared_ptr<Monkey> lhs, std::shared_ptr<Monkey> rhs)->std::int64_t
		{
			std::cout << "x";
			return 0;
		});
	pairMonkeys.first->GetNum();
	std::cout << "\n=\n";
	pairMonkeys.second->GetNum();

	//Take the output and use https://quickmath.com/webMathematica3/quickmath/equations/solve/basic.jsp to solve for x
}
