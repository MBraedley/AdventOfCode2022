#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <memory>
#include <set>

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

	auto operator<=>( const File& other )
	{
		return this->m_Name <=> other.m_Name;
	}

private:
	std::string m_Name{};
	std::size_t m_Filesize{};
};

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

	const std::set<std::unique_ptr<Dir>>& GetSubDirectories() const { return m_SubDirs; }

	void AddFile( const std::string& name, std::size_t size )
	{
		m_Files.emplace( name, size );
	}

	void AddDir( std::unique_ptr<Dir>&& dir )
	{
		m_SubDirs.insert( std::move( dir ) );
	}

	auto operator<=>( const Dir& other )
	{
		return this->m_Name <=> other.m_Name;
	}

private:
	std::string m_Name{};
	std::set<std::unique_ptr<Dir>> m_SubDirs;
	std::set<File> m_Files;
};

int main()
{
	std::filesystem::path inputFile( "input.txt" );
	std::ifstream inStrm( inputFile );

	std::string line;
	Dir root;
}
