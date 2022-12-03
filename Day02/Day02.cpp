#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <compare>

enum class RPS : int
{
    Rock,
    Paper,
    Scissors,
};

std::strong_ordering operator<=>(const RPS& lhs, const RPS& rhs)
{
    int l = static_cast<int>(lhs);
    int r = static_cast<int>(rhs);

    int result = l - r;
    switch (result)
    {
    case -2:
    case 1:
        return std::strong_ordering::greater;
    case 0:
        return std::strong_ordering::equal;
    case -1:
    case 2:
        return std::strong_ordering::less;
    default:
        throw std::exception("something went wrong");
    }
}

int main()
{
    std::filesystem::path inputFile("input.txt");
    std::ifstream inStrm(inputFile);

    std::vector<std::string> matches;
    std::string line;
    while (std::getline(inStrm, line))
    {
        matches.push_back(line);
    }

    std::uint64_t score = 0;
    for (const auto& match : matches)
    {
        RPS elf, me;
        switch (match[0])
        {
        case 'A':
            elf = RPS::Rock;
            break;
        case 'B':
            elf = RPS::Paper;
            break;
        case 'C':
            elf = RPS::Scissors;
            break;
        default:
            throw std::exception("something went wrong");
        }

        switch (match[2])
        {
        case 'X':
            me = RPS::Rock;
            break;
        case 'Y':
            me = RPS::Paper;
            break;
        case 'Z':
            me = RPS::Scissors;
            break;
        default:
            throw std::exception("something went wrong");
        }

        switch (me)
        {
        case RPS::Rock:
            score += 1;
            break;
        case RPS::Paper:
            score += 2;
            break;
        case RPS::Scissors:
            score += 3;
            break;
        default:
            throw std::exception("something went wrong");
        }

        int l = static_cast<int>(elf);
        int r = static_cast<int>(me);

        int result = l - r;
        switch (result)
        {
        case -2:
        case 1:
            score += 0;
            break;
        case 0:
            score += 3;
            break;
        case -1:
        case 2:
            score += 6;
            break;
        default:
            throw std::exception("something went wrong");
        }
    }

    std::cout << score << "\n";

    // Part 2
    score = 0;
    for (const auto& match : matches)
    {
        if (match == "A X")
        {
            score += 0ll + 3;
        }
        else if (match == "A Y")
        {
            score += 3ll + 1;
        }
        else if (match == "A Z")
        {
            score += 6ll + 2;
        }
        else if (match == "B X")
        {
            score += 0ll + 1;
        }
        else if (match == "B Y")
        {
            score += 3ll + 2;
        }
        else if (match == "B Z")
        {
            score += 6ll + 3;
        }
        else if (match == "C X")
        {
            score += 0ll + 2;
        }
        else if (match == "C Y")
        {
            score += 3ll + 3;
        }
        else if (match == "C Z")
        {
            score += 6ll + 1;
        }
    }

    std::cout << score << "\n";
}
