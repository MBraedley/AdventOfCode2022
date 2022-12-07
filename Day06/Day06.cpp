#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <set>

int main()
{
	std::filesystem::path inputFile( "input.txt" );
	std::vector<char> data;
	std::ifstream inStrm( inputFile );

	std::set<char> possibleHeader;
	do
	{
		char c;
		inStrm >> c;
		data.push_back( c );
		possibleHeader.clear();
		if( data.size() >= 4 )
		{
			possibleHeader.insert( data.rbegin(), data.rbegin() + 4 );
		}
	} while( possibleHeader.size() != 4 );

	std::cout << data.size() << "\n";

	//Part 2

	do
	{
		char c;
		inStrm >> c;
		data.push_back( c );
		possibleHeader.clear();
		if( data.size() >= 14 )
		{
			possibleHeader.insert( data.rbegin(), data.rbegin() + 14 );
		}
	} while( possibleHeader.size() != 14 );

	std::cout << data.size() << "\n";
}
