#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>

namespace Colors {
	constexpr auto None = 0x00;
	constexpr auto Blue = 0x01;
	constexpr auto Green = 0x02;
	constexpr auto Red = 0x04;
	constexpr auto All = 0x07;
}

auto	channel( const cv::Mat & mat, unsigned int index ) -> cv::Mat {
	if ( mat.empty() || index > static_cast<unsigned int>(mat.channels() - 1) ) {
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

auto	saveFrame( const cv::Mat & mat, const std::string & dir, const std::string & name, const std::string & extension ) {
	auto path = dir + "/" + name + "." + extension;
	cv::imwrite( path, mat );
	return path;
}

auto	detect( const std::string & file, const std::string & saveTo, double k ) -> int {
	cv::VideoCapture capture{ file };
	if( !capture.isOpened() ) {
		return -1;
	}

	auto frameCount = capture.get( CV_CAP_PROP_FRAME_COUNT );
	cv::BackgroundSubtractorMOG2 subtractor;

	auto i = 0;
	//auto isDetected = false;
	while ( true ) {
		++i;
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
		//auto isStrike = isDetected;
		auto isDetected = i > 60 && count > k * s;
		//isStrike = isStrike && isDetected;
		//auto isNewStrike = isDetected && !isStrike;
		
		//out += asColor( mask, Colors::Red );
		//cv::imshow( "out", out );

		if ( isDetected ) {
			cv::putText( out, "Detected", { 50, 50 }, cv::FONT_HERSHEY_COMPLEX, 1.0, { 0, 0, 255 } );
			auto time = capture.get( CV_CAP_PROP_POS_MSEC );
			auto outPath = saveFrame( out, saveTo, time2HMSString( time ), "jpg" );
			std::cout << "Save file as: " << outPath << std::endl;
		}

		if ( i % 1000 == 0 ) {
			auto percent = ( 100.0 * i ) / frameCount;
			std::cout << "Progress: " << asString( percent ) << "%" << std::endl;
		}
		cv::waitKey( 1 );
	}

	return 0;
}
