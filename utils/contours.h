#pragma once
#include <limits>
#include <opencv2/opencv.hpp>

typedef std::vector< cv::Point > Contour;
typedef std::vector< Contour > Contours;

auto	maskToContour( const cv::Mat & mask ) -> std::vector< cv::Point >;
auto	contoursArea( const cv::Mat & mat, double thresh = 30.0 ) -> double;
auto	getOptimalThresh( const cv::Mat & mat1, const cv::Mat & mat2, double smin, double thresh_min, double thresh_max ) -> double;

template < typename T1 >
auto	contourWidth( const std::vector< cv::Point_< T1 > > & contour ) -> T1 {
	if ( contour.empty() ) {
		return 0;
	}

	auto min = std::numeric_limits< T1 >::min();
	auto max = std::numeric_limits< T1 >::max();
	for ( auto && point : contour ) {
		min = std::max( min, point.x );
		max = std::min( max, point.x );
	}

	return std::abs( max - min );
}

template < typename T1 >
auto	contourHeight( const std::vector< cv::Point_< T1 > > & contour ) -> T1 {
	if ( contour.empty() ) {
		return 0;
	}

	auto min = std::numeric_limits< T1 >::min();
	auto max = std::numeric_limits< T1 >::max();
	for ( auto && point : contour ) {
		min = std::max( min, point.y );
		max = std::min( max, point.y );
	}

	return std::abs( max - min );
}