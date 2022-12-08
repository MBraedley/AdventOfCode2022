#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <memory>
#include <set>
#include <stack>
#include <regex>
#include <functional>

class File
{
public:
	File() = default;
	File( const std::string& name, std::size_t size ):
		m_Name( name ),
		m_Filesize( size )
	{}

	~File() = default;

	const std::string& GetName() const { return m_Name; }
	std::size_t GetSize() const { return m_Filesize; }

	friend auto operator<=>( const File& lhs, const File& rhs );
private:
	std::string m_Name{};
	std::size_t m_Filesize{};
};

auto operator<=>( const File& lhs, const File& rhs )
{
	return lhs.m_Name <=> rhs.m_Name;
}

class Dir
{
public:
	Dir() = default;
	explicit Dir( const std::string& name ):
		m_Name( name )
	{}

	~Dir() = default;

	const std::string& GetName() const { return m_Name; }
	std::size_t GetSize() const
	{
		std::size_t filesize = 0;
		for( const auto& file : m_Files )
		{
			filesize += file.GetSize();
		}

		for( const auto& folder : m_SubDirs )
		{
			filesize += folder->GetSize();
		}

		return filesize;
	}

	const std::set<std::shared_ptr<Dir>>& GetSubDirectories() const { return m_SubDirs; }

	std::shared_ptr<Dir> GetSubDirectory( std::string name )
	{
		auto iter = std::find_if( m_SubDirs.begin(), m_SubDirs.end(), [&]( const std::shared_ptr<Dir>& other )
			{
				return name == other->GetName();
			} );

		return *iter;
	}

	void AddFile( const std::string& name, std::size_t size )
	{
		m_Files.emplace( name, size );
	}

	void AddDir( std::shared_ptr<Dir> dir )
	{
		m_SubDirs.insert( dir );
	}

	friend auto operator<=>( const Dir& lhs, const Dir& rhs );
	friend auto operator<=>( const std::shared_ptr<Dir>& lhs, const std::shared_ptr<Dir>& rhs );

private:
	std::string m_Name{};
	std::set<std::shared_ptr<Dir>> m_SubDirs;
	std::set<File> m_Files;
};

auto operator<=>( const Dir& lhs, const Dir& rhs )
{
	return lhs.m_Name <=> rhs.m_Name;
}

auto operator<=>( const std::shared_ptr<Dir>& lhs, const std::shared_ptr<Dir>& rhs )
{
	return *lhs <=> *rhs;
}

std::uint64_t GetMatchingContents( std::shared_ptr<Dir> root, std::function<bool( std::size_t )> pred )
{
	std::uint64_t ret = 0;
	auto currentDirSize = root->GetSize();
	if( pred( currentDirSize ) )
	{
		ret += currentDirSize;
	}

	for( auto dir : root->GetSubDirectories() )
	{
		ret += GetMatchingContents( dir, pred );
	}

	return ret;
}

void GetSmallestLargeEnoughDir( std::shared_ptr<Dir> root, const std::size_t minSize, std::size_t& currentBest )
{
	if( auto dirSize = root->GetSize(); dirSize <= currentBest && dirSize >= minSize )
	{
		currentBest = dirSize;
	}

	for( auto dir : root->GetSubDirectories() )
	{
		GetSmallestLargeEnoughDir( dir, minSize, currentBest );
	}
}

int main()
{
	std::filesystem::path inputFile( "input.txt" );
	std::ifstream inStrm( inputFile );

	std::string line;
	std::shared_ptr<Dir> root = std::make_shared<Dir>( "/" );

	std::stack<std::shared_ptr<Dir>> dirStack;
	dirStack.push( root );

	std::regex cdCmd( "^\\$ cd (.+)$" );
	std::regex lsCmd( "^\\$ ls$" );
	std::regex lsDir( "^dir (.+)$" );
	std::regex lsFile( "^(\\d+) (.+)$" );

	std::smatch m;

	while( std::getline(inStrm, line) )
	{
		if( std::regex_match( line, m, cdCmd ) )
		{
			if( m[1] == ".." )
			{
				dirStack.pop();
			}
			else if ( m[1] != "/" )
			{
				dirStack.push( dirStack.top()->GetSubDirectory( m[1] ) );
			}
		}
		else if( std::regex_match( line, m, lsCmd ) )
		{
			//Do nothing
		}
		else if( std::regex_match( line, m, lsDir ) )
		{
			dirStack.top()->AddDir( std::make_shared<Dir>( m[1] ) );
		}
		else if ( std::regex_match(line, m, lsFile) )
		{
			dirStack.top()->AddFile( m[2], std::stoull( m[1] ) );
		}
	}

	std::uint64_t smallFolderSizes = GetMatchingContents( root, []( std::size_t folderSize ) -> bool
		{
			return folderSize <= 100000;
		} );

	std::cout << smallFolderSizes << "\n";

	// Part 2
	constexpr std::size_t hddSize{ 70000000 };
	constexpr std::size_t hddSpaceNeeded{ 30000000 };

	std::size_t spaceAvailable = hddSize - root->GetSize();
	const std::size_t minDirSizeToDelete = hddSpaceNeeded - spaceAvailable;

	std::size_t bestSize = root->GetSize();
	GetSmallestLargeEnoughDir( root, minDirSizeToDelete, bestSize );
	std::cout << bestSize << "\n";
}
