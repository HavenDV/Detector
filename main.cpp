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

auto	detect_file_or_url( const fs::path & file, const fs::path & saveTo, double k ) {
	auto isExists = fs::exists( file );
	auto isUrl = is_url( file );
	if ( !isExists && !isUrl ) {
		std::cout << "File or directory " << file << " not is exists" << std::endl;
		return -1;
	}

	auto outDir = saveTo.empty() ? ( file.has_parent_path() ? file.parent_path() : "./" ) : saveTo;
	if ( !isUrl ) {
		outDir /= file.stem();
	}

	if ( !fs::exists( outDir ) && !fs::create_directories( outDir ) ) {
		std::cout << "Unable to create directory " << outDir << std::endl;
		return -1;
	}

	std::cout << "Begin. k = " << k << "." << std::endl;
	std::cout << "Input " << ( isUrl ? "url" : "file" ) << " is " << file << std::endl;
	std::cout << "Files save to " << outDir << std::endl;
	
	return detect( file.string(), outDir.string(), k );
}

auto	detect_directory( const fs::path & file, const fs::path & outDir, double k ) {
	std::cout << "Start work in directory " << file << std::endl;
	for ( fs::directory_iterator it( file ), end; it != end; ++it) {
		if ( it->path().extension() != ".ts" ) {
			continue;
		}

		detect_file_or_url( it->path(), outDir, k );
	}

	return 0;
} 

int	main( int argc, char** argv ) {
	auto k = 0.1;
	fs::path outDir;
	std::vector< fs::path > input;

	po::options_description desc( "Allowed options" );
	desc.add_options()
	( "help", "this help message" )
	( "k", po::value( &k ), "detection relative area. Default is 0.1" )
	( "o", po::value( &outDir ), "out path. Default is fileDir or currentDir" )
	( "input", po::value( &input ), "input files, dirs or urls" );

	po::positional_options_description p;
	p.add( "input", -1 );

	po::variables_map vm;
	po::store( po::command_line_parser( argc, argv ).options( desc ).positional( p ).run(), vm );
	po::notify( vm );

	if ( vm.count( "help" ) || argc < 2 ) {
		std::cout << "This program detect all moves in video" << std::endl;
    		std::cout << desc << "\n";
    		return 1;
	}

	try {
		for ( auto && file : input ) {
			if ( !fs::is_directory( file ) ) {
				detect_file_or_url( file, outDir, k );
			}	
			else {
				detect_directory( file, outDir, k );
			}
		}
	} 
	catch ( const std::exception & exception ) {
		std::cerr << "Error: "<< exception.what() << std::endl;
		return -1;
	}

	return 0;
}
