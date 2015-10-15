#include "utils.h"
#include "contours.h"

auto	contoursArea( const cv::Mat & mat, double thresh )
-> double {
	if ( mat.empty() ) {
		return 0.0;
	}
	
	threshold( mat, mat, thresh, 255, cv::THRESH_BINARY );
	//adaptiveThreshold(in, in, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 3, 1);

	Contours contours;
	findContours( toGray( mat ), contours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_L1 );

	auto data = 0.0;
	for ( auto && i : contours ) {
		cv::approxPolyDP( i, i, 1.0, false );
		//convexHull( i, i );
		data += cv::contourArea( i );
		//data += arcLength( i, 0 );
	}

	/*
	for ( auto && i : contours ) {
		cv::drawContours( mat, i, { 255, 255, 255 } );
	}
	*/
	return data;
}

auto	maskToContour( const cv::Mat & mat )
-> Contour {
	Contours contours;
	cv::findContours( toGray( mat ), contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE );
	if ( !contours.empty() ) {
		return contours[ 0 ];
	}
	return{};
}

auto	getOptimalThresh( const cv::Mat & mat1, const cv::Mat & mat2, double smin, double thresh_min, double thresh_max )
-> double {
	auto thresh = thresh_max, s = 0.0;
	for ( ; s < smin && thresh >= thresh_min; --thresh ) {
		s = contoursArea( cv::abs( mat1 - mat2 ), thresh );
	}
	return thresh;
}