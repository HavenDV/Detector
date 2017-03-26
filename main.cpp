#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "detect.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

bool	is_url( const fs::path & path ) {
	auto string = path.string();
	auto found = string.find( "://" );
	if ( found == std::string::npos ) {
		return false;
	}

	return true;
}

void	detect_file_or_url( const fs::path & file, const fs::path & to, double k ) {
	auto isExists = fs::exists( file );
	auto isUrl = is_url( file );
	if ( !isExists && !isUrl )
	{
		throw std::exception(("File or directory " + file.string() + " not is exists").c_str());
	}

	auto toDir = to;
	if (toDir.empty())
	{
		toDir = isUrl ? fs::current_path() : (file.has_parent_path() ? file.parent_path() : "./");
	}
	else
	{
		if (!fs::exists(toDir) && !fs::create_directories(toDir))
		{
			throw std::exception(("Unable to create directory " + toDir.string()).c_str());
		}
	}

	std::cout << "Begin. k = " << k << "." << std::endl;
	std::cout << "Input " << ( isUrl ? "url" : "file" ) << " is " << file << std::endl;
	std::cout << "Files save to " << toDir << std::endl;
	
	detect( file.string(), toDir.string(), k );
}

static const std::vector<std::string> availableExtensions = {
	".ts", ".avi", ".mp4",
	".TS", ".AVI", ".MP4"
};

bool	isVideo(const fs::path & path)
{
	for (auto extension : availableExtensions)
	{
		if (path.extension() == extension)
		{
			return true;
		}
	}

	return false;
}

void	detect_directory( const fs::path & file, const fs::path & outDir, double k ) {
	std::cout << "Start work in directory " << file << std::endl;
	for ( fs::directory_iterator iterator( file ), end; iterator != end; ++iterator)
	{
		isVideo(file) ? detect_file_or_url(iterator->path(), outDir, k) : 0;
	}
} 

int	main( int argc, char** argv ) {
	auto k = 0.1;
	fs::path to;
	std::vector< fs::path > input;

	po::options_description desc( "Allowed options" );
	desc.add_options()
	( "help", "this help message" )
	( "k", po::value( &k ), "detection relative area. Default is 0.1" )
	( "o", po::value( &to ), "out path. Default is fileDir or currentDir" )
	( "input", po::value( &input ), "input files, dirs or urls" );

	po::positional_options_description p;
	p.add( "input", -1 );

	po::variables_map vm;
	po::store( po::command_line_parser( argc, argv ).options( desc ).positional( p ).run(), vm );
	po::notify( vm );

	if ( vm.count( "help" ) || argc < 2 )
	{
		std::cout << "This program detect all moves in video" << std::endl;
		std::cout << desc << "\n";
		return 0;
	}
	
	try {
		for ( auto && file : input )
		{
			if ( fs::is_directory( file ) )
			{
				detect_directory(file, to, k);
			}	
			else
			{
				detect_file_or_url(file, to, k);
			}
		}
	} 
	catch ( const std::exception & exception ) {
		std::cerr << "Error: "<< exception.what() << std::endl;
		return -1;
	}

	return 0;
}
