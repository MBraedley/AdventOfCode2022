#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <functional>
#include <set>

std::int64_t CalculateScenicScore( const std::vector<std::vector<std::int8_t>>& forest, std::int32_t x, std::int32_t y );

int main()
{
	std::filesystem::path inputFile("input.txt");
	std::vector<std::vector<std::int8_t>> forest;
	std::ifstream inStrm(inputFile);

	std::string line;
	std::uint64_t calCount = 0;
	while (std::getline(inStrm, line))
	{
		std::vector<std::int8_t> row;
		std::transform( line.begin(), line.end(), std::back_inserter( row ), []( const char& c ) -> std::int8_t
			{
				return c - '0';
			} );

		forest.push_back( row );
	}

	std::set<std::pair<std::size_t, std::size_t>> visibleTrees;

	auto checkVisibilty = [&]( auto x, auto y, auto& heightLimit )
	{
		if( forest[y][x] > heightLimit )
		{
			visibleTrees.emplace( x, y );
			heightLimit = forest[y][x];
		}
	};

	std::int8_t heightLimit;
	for( std::size_t y = 0; y < forest.size(); y++ )
	{
		heightLimit = -1;
		for( std::size_t x = 0; x < forest[y].size(); x++ )
		{
			checkVisibilty( x, y, heightLimit );
		}

		heightLimit = -1;
		for( std::int32_t x = forest[y].size() - 1; x >= 0; x-- )
		{
			checkVisibilty( x, y, heightLimit );
		}
	}

	for( std::size_t x = 0; x < forest[0].size(); x++ )
	{
		heightLimit = -1;
		for( std::size_t y = 0; y < forest.size(); y++ )
		{
			checkVisibilty( x, y, heightLimit );
		}

		heightLimit = -1;
		for( std::int32_t y = forest.size() - 1; y >= 0; y-- )
		{
			checkVisibilty( x, y, heightLimit );
		}
	}

	std::cout << visibleTrees.size() << "\n";

	//Part 2
	std::int64_t bestScore = 0;
	for( std::int32_t y = 0; y < forest.size(); y++ )
	{
		for( std::int32_t x = 0; x < forest[y].size(); x++ )
		{
			if( auto score = CalculateScenicScore( forest, x, y ); score > bestScore )
			{
				bestScore = score;
			}
		}
	}

	std::cout << bestScore << "\n";
}

std::int64_t CalculateScenicScore( const std::vector<std::vector<std::int8_t>>& forest, std::int32_t x, std::int32_t y )
{
	auto startingTree = forest[y][x];

	std::int64_t nScore = 1;
	while( y - nScore >= 0 && startingTree > forest[y - nScore][x] )
	{
		nScore++;
	}

	if( y - nScore < 0 )
	{
		nScore--;
	}

	if( nScore == 0 )
	{
		return 0;
	}

	std::int64_t sScore = 1;
	while( y + sScore < forest.size() && startingTree > forest[y + sScore][x] )
	{
		sScore++;
	}

	if( y + sScore >= forest.size() )
	{
		sScore--;
	}

	if( sScore == 0 )
	{
		return 0;
	}

	std::int64_t wScore = 1;
	while( x - wScore >= 0 && startingTree > forest[y][x - wScore] )
	{
		wScore++;
	}

	if( x - wScore < 0 )
	{
		wScore--;
	}

	if( wScore == 0 )
	{
		return 0;
	}

	std::int64_t eScore = 1;
	while( x + eScore < forest[y].size() && startingTree > forest[y][x + eScore] )
	{
		eScore++;
	}

	if( x + eScore >= forest[y].size() )
	{
		eScore--;
	}

	return nScore * sScore * wScore * eScore;
}
