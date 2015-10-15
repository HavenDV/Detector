#include "utils.h"

auto	operator "" _s( const char* str, size_t size ) -> cv::String {
	return str;
}

auto	generateIrisTemplate( const cv::Size & size ) -> cv::Mat {
	auto markers = cv::Mat{ size, CV_8U, cv::Scalar::all( cv::GC_BGD ) };
	cv::circle( markers, getCenterPoint( size ), size.height / 4,
				cv::Scalar::all( cv::GC_PR_FGD ), cv::FILLED );
	cv::circle( markers, getCenterPoint( size ), size.height / 8,
				cv::Scalar::all( cv::GC_FGD ), cv::FILLED );
	return markers;
}


auto	detectEyeMask( const cv::Mat & in, int arg1 ) -> cv::Mat {
	return morph( toGray( in ), 6, 5 ) > arg1;
}

auto	toGray( const cv::Mat & mat, bool normalise ) -> cv::Mat {
	if ( mat.empty() ) {
		return mat;
	}

	cv::Mat gray;
	if ( mat.channels() == 4 ) {
		cvtColor( mat, gray, cv::COLOR_BGRA2GRAY );
	}
	else if ( mat.channels() == 3 ) {
		cvtColor( mat, gray, cv::COLOR_BGR2GRAY );
	}
	else {
		gray = mat;
	}


	if ( normalise && gray.channels() == 1 ) {
		cv::Mat out;
		equalizeHist( gray, out );
		return out;
	}

	return gray;
}

auto	operator !( const cv::Mat & mat ) -> cv::Mat {
	return cv::Scalar::all( 255 ) - mat;
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

void	imshow_s( const std::string & name, cv::InputArray & mat ) {
	if ( mat.empty() || name.empty() ) {
		return;
	}

	cv::imshow( name, mat );
}

void	resizeWindow( const std::string & name, const cv::Size & size ) {
	cv::resizeWindow( name, size.width, size.height );
}

void	moveWindow( const std::string & name, const cv::Point & position ) {
	cv::moveWindow( name, position.x, position.y );
}

std::vector< std::string > createdWindows;

void	createWindow( const std::string & name, const cv::Point & position, const cv::Size & size, int flags ) {
	if ( std::find( createdWindows.begin(), createdWindows.end(), name ) != createdWindows.end() ) {
		return;
	}

	cv::namedWindow( name, flags ); // 32 no gui expand ( 1 << 5 ) & 	 36
	moveWindow( name, position );
	resizeWindow( name, size );
	createdWindows.insert( std::end( createdWindows ), name );
}

void	destroyCreatedWindows() {
	for ( auto && name : createdWindows ) {
		cv::destroyWindow( name );
	}
}

auto	randomColor() -> cv::Scalar_< int > {
	return{ std::rand() % 255, std::rand() % 255, std::rand() % 255 };
}

void	showTemplate( const std::string & name, const cv::Mat & templateMat ) {
	cv::Mat_< cv::Vec3b > out{ templateMat.size() };
	double minValue, maxValue;
	cv::minMaxLoc( templateMat, &minValue, &maxValue );
	auto absMaxValue = std::max( std::abs( minValue ), std::abs( maxValue ) );
	out.forEach( [ & ] ( auto && color, auto && position ) {
		auto x = position[ 1 ];
		auto y = position[ 0 ];

		auto templateValue = templateMat.at< float >( position );
		if ( templateValue > 0.0F ) {
			color = { static_cast< uchar >( templateValue * 255 / absMaxValue ), 0, 0 };
		}
		else {
			color = { 0, 0, static_cast< uchar >( -templateValue * 255 / absMaxValue ) };
		}
	} );
	createWindow( name, {} );
	imshow_s( name, out );
}

auto	filter( const cv::Mat & mat ) -> cv::Mat {
	if ( mat.empty() ) {
		return mat;
	}

	cv::Mat out;
	//blur
	//GaussianBlur( in, out, Size(7,7), 1 );
	//medianBlur
	//adaptiveBilateralFilter( in, out, Size( 5, 5 ), 5, 100.0 );
	cv::bilateralFilter( mat, out, 9, 100, 5 );
	return out;
}

auto	resize( const cv::Mat & mat, double k ) -> cv::Mat { // <0.5x CPU speed in GPU.
	if ( mat.empty() ) {
		return mat;
	}

	cv::Mat out;
	cv::resize( mat, out, k * mat.size(), 0, 0, k > 1.0 ? cv::INTER_CUBIC : cv::INTER_AREA );
	return out;
}

auto	morph( const cv::Mat & mat, int type, int arg ) -> cv::Mat {
	if ( mat.empty() ) {
		return mat;
	}

	auto out = cv::Mat{};
	auto element = cv::getStructuringElement( 0, { arg * 2 + 1, arg * 2 + 1 }, { arg, arg } );

	type %= 7;

	type < 2 ? type < 1 ?
		cv::erode( mat, out, element ) :
		cv::dilate( mat, out, element ) :
		cv::morphologyEx( mat, out, type, element );

	return out;
}

auto	getSFlowPoint( const cv::Point & point, const cv::Mat & flow, int step )
-> cv::Point2d {
	auto stepPoint = cv::Point{ cvRound( step / 2 ), cvRound( step / 2 ) };
	auto rect = clamp( cv::Rect{ point - stepPoint, point + stepPoint }, flow );
	auto mean = cv::mean( flow( rect ) );
	return{ mean[ 0 ], mean[ 1 ] };
}

void	drawOptFlow( cv::Mat & mat, const cv::Mat & flow, int step,
					 double k, const cv::Scalar & color ) {
	for ( int y = cvRound( step / 2 ); y < mat.rows; y += step ) {
		for ( int x = cvRound( step / 2 ); x < mat.cols; x += step ) {
			//auto && fxy = flow.at<Point2f>(y, x);
			auto fxy = getSFlowPoint( { x, y }, flow, step );
			line( mat, { x, y }, { x + cvRound( k * fxy.x ), y + cvRound( k * fxy.y ) }, color );
		}
	}
}