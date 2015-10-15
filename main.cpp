#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace Colors {
	constexpr auto None = 0x00;
	constexpr auto Blue = 0x01;
	constexpr auto Green = 0x02;
	constexpr auto Red = 0x04;
	constexpr auto All = 0x07;
}

auto	channel( const cv::Mat & mat, int index ) -> cv::Mat {
	if ( mat.empty() || index > mat.channels() - 1 ) {
		return cv::Mat::zeros( mat.size(), mat.type() & cv::NORM_TYPE_MASK );
	}

	auto channels = std::vector< cv::Mat >{};
	cv::split( mat, channels );

	if ( index >= channels.size() ) {
		return{};
	}

	return channels[ index ];
}

auto	asColor( const cv::Mat & mat, int color ) -> cv::Mat {
	if ( mat.empty() ) {
		return mat;
	}

	auto channels = std::vector<cv::Mat>{};
	for ( int i = 0; i < 3; i++ ) {
		channels.insert( channels.end(), color & ( 1 << i ) ?
						 channel( mat, 0 ) :
						 cv::Mat::zeros( mat.size(), mat.type() & cv::NORM_TYPE_MASK ) );
	}

	auto out = cv::Mat{};
	cv::merge( channels, out );

	return out;
}

template < typename T1, typename T2 >
auto	round( const T1 data, const T2 size = 0.01 )
-> decltype( data * size ) {
	return size * std::round( ( 1 / size ) * data );
}

auto	toString( double value )
-> std::string {
	auto string = std::to_string( round( value, 0.01 ) );
	string.erase( string.begin() + ( string.size() - 4 ), string.end() );
	return string;
}

template < typename T1 >
auto	asString( T1 value )
-> std::string {
	std::ostringstream stream;
	stream << value;
	return stream.str();
}

template <>
auto	asString( double value )
-> std::string {
	std::ostringstream stream;
	stream << toString( value );
	return stream.str();
}

template < typename T1, typename ... Args >
auto	asString( T1 value, Args ... args )
-> std::string {
	return asString( value ) + asString( args ... );
}

auto int2Time( int value )
-> std::string {
	auto string = asString( value );
	if ( value >= 10 || value < 0 ) {
		return string;
	}
	return "0" + string;
}

auto time2HMSString( double time ) {
	auto s = static_cast< int >( std::floor( time / 1000 ) ) % 60;
	auto m = static_cast< int >( std::floor( time / 1000 / 60 ) ) % 60;
	auto h = static_cast< int >( std::floor( time / 1000 / 60 / 60 ) ) % 24;
	return int2Time( h ) + "-" + int2Time( m ) + "-" + int2Time( s );
}

auto	createDir( const std::string & dir, const std::string & name = "" ) {
	auto path = dir + "/" + name;
	//std::system( ( "mkdir -p \"" + path + "\"" ).c_str() );
	fs::create_directory( path );
	return path;
}

auto	saveFrame( const cv::Mat & mat, const std::string & dir, const std::string & name, const std::string & extension ) {
	auto path = dir + "/" + name + "." + extension;
	cv::imwrite( path, mat );
	return path;
}

auto	getFileName( const std::string & path )
-> std::string {
	auto found = path.find_last_of( "/\\" );
	return path.substr( found + 1 );
}

auto	getFileNameWithoutExtension( const std::string & path )
-> std::string {
	auto name = getFileName( path );
	auto found = name.find_last_of( '.' );
	if ( found == std::string::npos ) {
		return path;
	}

	return name.substr( 0, found );
}

auto	getFileExtension( const std::string & path )
-> std::string {
	auto found = path.find_last_of( '.' );
	if ( found == std::string::npos ) {
		return "";
	}

	return path.substr( found + 1 );
}

auto	getFileDir( const std::string & path )
-> std::string {
	auto found = path.find_last_of( "/\\" );
	if ( found == std::string::npos ) {
		return "";
	}

	return path.substr( 0, found );
}

auto	detect( const std::string & file, const std::string & saveTo, double k ) {
	cv::VideoCapture capture{ file };
	auto frameCount = capture.get( CV_CAP_PROP_FRAME_COUNT );
	//CV_Assert( !capture.isOpened() );

	cv::BackgroundSubtractorMOG2 subtractor;

	//createWindow( "out", { 0, 0 }, { 800, 600 }, cv::WINDOW_NORMAL );

	auto i = 0;
	auto isDetected = false;
	//auto lastDir = std::string( "" );
	while ( true ) {
		++i;
		//capture.set( cv::CAP_PROP_POS_FRAMES, i );
		cv::Mat frame;
		capture >> frame;
		if ( i % 30 != 0 ) {
			continue;
		}
		if ( frame.empty() ) {
			break;
		}

		cv::Mat mask;
		subtractor( frame, mask );

		auto out = frame.clone();
		auto s = frame.size().area();
		auto count = cv::countNonZero( mask );
		auto isStrike = isDetected;
		isDetected = i > 60 && count > k * s;
		isStrike = isStrike && isDetected;
		auto isNewStrike = isDetected && !isStrike;
		
		cv::putText( out, isDetected ? "Detected" : "", { 50, 50 }, cv::FONT_HERSHEY_COMPLEX, 1.0, { 0, 0, 255 } );
		//out += asColor( mask, Colors::Red );
		//cv::imshow( "out", out );
		//writer << out;

		if ( isDetected ) {
			auto time = capture.get( CV_CAP_PROP_POS_MSEC );
			//if ( isNewStrike ) {
			//	lastDir = createDir( saveTo, timeToHMSString( time ) );
			//}
			auto outPath = saveFrame( out, saveTo, time2HMSString( time ), "jpg" );
			std::cout << "Save file as: " << outPath << std::endl;
		}

		if ( i % 1000 == 0 ) {
			auto percent = 100.0 * i / frameCount;
			if ( percent > 30.0 ) {
				return 0;
			}
			std::cout << "Progress: " << asString( percent ) << "%" << std::endl;
		}
		cv::waitKey( 1 );
		
		//i += 30;
	}

	return 0;
}

static void help() {
	std::cout << std::endl <<
			"This program detect all moves in video\n"
			"Call\n"
			"detect [ -k value -- default is 0.1 ] [-o path -- out dir. default is fileDir or currentDir ] file_name.ts\n\n";
}

int	main( int argc, char** argv ) {
	if ( argc < 2 ) {
		help();
		return 0;
	}
	auto k = 0.1;
	auto outDir = std::string( "" );
	for ( auto i = 0; i < argc; ++i ) {
		if ( std::string( argv[ i ] ) == "-k" && i + 1 < argc ) {
			k = std::atof( argv[ i + 1 ] );
			++i;
		}
		else if ( std::string( argv[ i ] ) == "-o" && i + 1 < argc ) {
			outDir = argv[ i + 1 ];
			if ( outDir[ outDir.size() - 1 ] == '/' ) {
				outDir = getFileDir( outDir );
			}
			++i;
		}
	}
	auto currentDir = getFileDir( argv[ 0 ] );
	auto filePath = argv[ argc - 1 ];
	auto file = fs::path{ filePath };
	auto isDir = fs::is_directory( file );
	auto isExists = fs::exists( file );
	if ( !isExists ) {
		std::cout << "File or directory " << file << " not is exists" << std::endl;
		return -1;
	}
	std::cout << "Is directory: " << isDir << std::endl;
	auto fileName = getFileNameWithoutExtension( filePath );
	auto fileDir = getFileDir( filePath );
	auto saveTo = outDir == "" ? createDir( fileDir == "" ? currentDir : fileDir, fileName ) : createDir( outDir );
	std::cout << "Begin. k = " << k << ". Files save to \"" << saveTo << "\"" << std::endl;
	try {	
		return detect( filePath, saveTo, k );
	} 
	catch ( ... ) { //const std::exception & exception
		//std::cerr << exception.what();
		std::cin.ignore();
		return -1;
	}
}
