#pragma once
#include <mutex>
#include <opencv2/opencv.hpp>

namespace Colors {
	constexpr auto None = 0x00;
	constexpr auto Blue = 0x01;
	constexpr auto Green = 0x02;
	constexpr auto Red = 0x04;
	constexpr auto All = 0x07;
}

/*
enum class Colors : int {
None = 0x00,
Blue = 0x01,
Green = 0x02,
Red = 0x04,
All = 0x07
};
*/

auto	operator "" _s( const char* str, size_t size )->cv::String;

auto	toGray( const cv::Mat & in, bool normalise = false )->cv::Mat;

void	findBigEye( const cv::Mat & mat, int arg1, int arg2 );
bool	findSmallEye( const cv::Mat & in, const cv::Mat & out, int arg2, const cv::Vec3f & bigCircle, float k = 1.0F );


auto	operator !( const cv::Mat & mat )->cv::Mat;
auto	channel( const cv::Mat & mat, int index )->cv::Mat;
auto	asColor( const cv::Mat & mat, int color )->cv::Mat;
void	imshow_s( const std::string & name, cv::InputArray & mat );

void	resizeWindow( const std::string & name, const cv::Size & size );
void	moveWindow( const std::string & name, const cv::Point & pos );
void	createWindow( const std::string & name, const cv::Point & pos, const cv::Size & size = cv::Size{ 640, 480 }, int flags = 0 );
void	destroyCreatedWindows();
void	showTemplate( const std::string & name, const cv::Mat & templateMat );

template< typename T1 >
auto	operator !( const cv::Mat_< T1 > & mat )
-> cv::Mat_< T1 > {
	return cv::Scalar::all( 255 ) - mat;
}

template < typename T1, typename T2 >
auto	round( const T1 data, const T2 size = 0.01 )
-> decltype( data * size ) {
	return size * std::round( ( 1 / size ) * data );
}

template < typename T1, typename T2 >
auto	operator * ( const T1 k, const cv::Size_<T2> & size )
-> cv::Size_< decltype( k * size.width ) > {
	return{ k * size.width, k * size.height };
}

template < typename T1, typename T2 >
auto	operator * ( const cv::Size_<T1> & size, const T2 k )
-> decltype( k * size ) {
	return k * size;
}
/*
template < typename T1, typename T2, typename T3, typename T4 >
auto	clamp( const T1 value, const T2 min, const T3 max )
-> T4 {
static_assert( std::is_convertible< T1, T4 >::value,
"clamp can't convert value type to out type." );
static_assert( std::is_convertible< T2, T4 >::value,
"clamp can't convert min type to out type." );
static_assert( std::is_convertible< T3, T4 >::value,
"clamp can't convert max type to out type" );

return std::min( std::max( static_cast< T4 >( value ), static_cast< T4 >( min ) ), static_cast< T4 >( max ) );
}
*/
template < typename T1, typename T2, typename T3 >
auto	clamp( const T1 value, const T2 min, const T3 max )
-> T1 {
	static_assert( std::is_convertible< T2, T1 >::value,
				   "clamp can't convert min type to value type." );
	static_assert( std::is_convertible< T3, T1 >::value,
				   "clamp can't convert max type to value type" );

	return std::min( std::max( value, static_cast< T1 >( min ) ), static_cast< T1 >( max ) );
}

template < typename T1, typename T2 >
auto	clamp( const cv::Point_< T1 > & point, const cv::Rect_< T2 > & rect )
-> cv::Point_< T1 > {
	return{ clamp( point.x, rect.x, rect.x + rect.width ),
		clamp( point.y, rect.y, rect.y + rect.height ) };
}

template < typename T1, typename T2, typename T3 >
auto	clamp( const cv::Point_< T1 > & point1, const cv::Point_< T2 > & point2, const cv::Rect_< T3 > & rect )
-> cv::Rect_< T1 > {
	return{ clamp( point1, rect ), clamp( point2, rect ) };
}

template < typename T1, typename T2 >
auto	clamp( const cv::Rect_< T1 > & rect1, const cv::Rect_< T2 > & rect2 )
-> cv::Rect_< T1 > {
	return clamp( rect1.tl(), rect1.br(), rect2 );
}

template < typename T1 >
auto	toRect( const cv::Size_< T1 > & size )
-> cv::Rect_< T1 > {
	return{ { 0, 0 }, size };
}

template < typename T1 = int >
auto	toRect( const cv::Mat & mat )
-> cv::Rect_< T1 > {
	return toRect< T1 >( mat.size() );
}

template < typename T1, typename T2 >
auto	clamp( const cv::Point_< T1 > & point, const cv::Size_< T2 > & size )
-> cv::Point_< T1 > {
	return clamp( point, toRect( size ) );
}

template < typename T1, typename T2, typename T3 >
auto	clamp( const cv::Point_< T1 > & point1, const cv::Point_< T2 > & point2, const cv::Size_< T3 > & size )
-> cv::Rect_< T1 > {
	return clamp( point1, point2, toRect( size ) );
}

template < typename T1, typename T2 >
auto	clamp( const cv::Rect_< T1 > & rect, const cv::Size_< T2 > & size )
-> cv::Rect_< T1 > {
	return clamp( rect, toRect( size ) );
}

template < typename T1 >
auto	clamp( const cv::Point_< T1 > & point, const cv::Mat & mat )
-> cv::Point_< T1 > {
	return clamp( point, mat.size() );
}

template < typename T1, typename T2 >
auto	clamp( const cv::Point_< T1 > & point1, const cv::Point_< T2 > & point2, const cv::Mat & mat )
-> cv::Rect_< T1 > {
	return clamp( point1, point2, mat.size() );
}

template < typename T1 >
auto	clamp( const cv::Rect_< T1 > & rect, const cv::Mat & mat )
-> cv::Rect_< T1 > {
	return clamp( rect, mat.size() );
}

template < typename T1 >
auto	inside( const cv::Rect_< T1 > & rect, const cv::Mat & mat )
-> bool {
	return rect == clamp( rect, mat );
}

template < typename T1, typename T2 = T1 >
auto	getCenterPoint( const cv::Rect_< T1 > & rect )
-> cv::Point_< T2 > {
	return 0.5F * ( rect.tl() + rect.br() );
}

template < typename T1, typename T2 = T1 >
auto	getCenterPoint( const cv::Size_< T1 > & size )
-> cv::Point_< T2 > {
	return getCenterPoint< T2 >( toRect( size ) );
}

template < typename T1, typename T2 = T1 >
auto	getCenterRect( const cv::Size_< T1 > & size )
-> cv::Rect_< T2 > {
	auto outSize = cv::Size_< T2 >( size ) / static_cast< T2 >( 2 );
	auto outPoint = cv::Point_< T2 >( cv::Point_< T1 >{ size.width, size.height } ) / static_cast< T2 >( 2 );
	return{ outPoint, outSize };
}

template < typename T1, typename T2 = T1 >
auto	getCenterRect( const cv::Rect_< T1 > & rect )
-> cv::Rect_< T2 > {
	auto outRect = getCenterRect< T2 >( rect.size() );
	outRect.tl = rect.tl;
	return outRect;
}

template < typename T1 = float >
auto	getCenterPoint( const cv::Mat & mat )
-> cv::Point_< T1 > {
	return getCenterPoint< T1 >( mat.size() );
}

template < typename T1 = float >
auto	getCenterRect( const cv::Mat & mat )
-> cv::Rect_< T1 > {
	return getCenterRect< T1 >( mat.size() );
}

template < typename T1 >
auto	getWeight( const T1 value )
-> float {
	return value > 0 ? 1.0F : 0.0F;
}

template < typename T1, int T2 >
auto	getWeight( const cv::Vec< T1, T2 > & vec )
-> float {
	typedef cv::Vec< T1, T2 > HVec;
	auto weight = 0.0F;
	for ( auto i = 0; i < HVec::channels; ++i ) {
		weight += getWeight( vec[ i ] );
	}
	return weight / HVec::channels;
}

//Center mass for binary image;
template < typename T1 >
auto	getCenterMass( const cv::Mat_< T1 > & mat )
-> cv::Point2f {
	auto fnum = 0.0F, sumx = 0.0F, sumy = 0.0F;
	constexpr auto full_koef = 3.0F;

	std::mutex mutex;
	mat.forEach( [ & ] ( const auto & color, const auto position ) {
		auto i = position[ 0 ];
		auto j = position[ 1 ];

		auto weight = getWeight( color );
		weight = weight < 1.0F ? weight / full_koef : weight;

		std::lock_guard< std::mutex > lock{ mutex };
		sumx += weight * j;
		sumy += weight * i;
		fnum += weight;
	} );

	if ( fnum > 0.0F ) {
		return clamp( cv::Point2f{ sumx / fnum, sumy / fnum }, mat );
	}

	return getCenterPoint( mat );
}

template < typename T1 >
auto	vectorSum( const T1 & value )
-> double {
	return static_cast< double >( value );
}

template < typename T1, int T2 >
auto	vectorSum( const cv::Vec< T1, T2 > & vec )
-> double {
	auto sum = 0.0;
	for ( auto i = 0; i < T2; ++i ) {
		sum += vec[ i ];
	}
	return sum;
}

//Center mass for non binary images.
//@min -> Only for points: vec[ 0 ] + vec[ ... ] + vec[ n ] > min
//@max -> Only for points: vec[ 0 ] + vec[ ... ] + vec[ n ] < max
template < typename T1 >
auto	getIntensivePoint( const cv::Mat_< T1 > & mat, T1 min = 0, T1 max = std::numeric_limits< T1 >::max() )
-> cv::Point2f {
	auto sum = 0ULL;
	std::mutex mutex;

	mat.forEach( [ & ] ( const auto & color, const auto position ) {
		auto colorSum = vectorSum( color );
		if ( colorSum >= min && colorSum <= max ) {
			std::lock_guard< std::mutex > lock{ mutex };
			sum += colorSum;
		}
	} );

	auto sumFloat = static_cast< float >( sum );
	auto pointSum = cv::Point2f{ 0.0F, 0.0F };

	if ( sumFloat < 1.0F ) {
		return getCenterPoint( mat );
	}

	mat.forEach( [ & ] ( const auto & color, const auto position ) {
		auto i = position[ 0 ];
		auto j = position[ 1 ];

		auto colorSum = vectorSum( color );
		if ( colorSum >= min && colorSum <= max ) {
			auto allColorFloat = static_cast< float >( colorSum );
			auto k = allColorFloat / sumFloat;

			std::lock_guard< std::mutex > lock{ mutex };
			pointSum += cv::Point2f{ k * j, k * i };
		}
	} );

	return clamp( pointSum, mat );
}

//Applyes mask and return sum all elements
template < typename T1 >
auto	compare( const cv::Mat_< T1 > & mat, const cv::Mat_< float > & mask )
-> float {
	auto sum = 0.0F;

	std::mutex mutex;
	mask.forEach( [ & ] ( auto && color, auto && position ) {
		if ( color == 0.0F ) {
			return;
		}

		auto value = color * vectorSum( mat.at< T1 >( position ) );

		std::lock_guard< std::mutex > locker{ mutex };
		sum += value;
	} );

	/*
	for ( auto x = 0; x < mat.cols; ++x ) {
	for ( auto y = 0; y < mat.rows; ++y ) {
	auto && color = mask.at< T1 >( { x, y } );
	if ( color == 0.0F ) {
	continue;
	}
	sum += color * vectorSum( mat.at< T1 >( { x, y } ) );
	}
	}
	*/
	return sum / mask.size().area();
}


//Find good rect for template
template < typename T1 >
void	findGoodRect( const cv::Mat_< T1 > & mat, const cv::Mat & mask, cv::Point & maxPoint, float & max, const cv::Mat & chanceMap = cv::Mat{} ) {
	auto expandPoint = cv::Point{ mask.cols / 2, mask.rows / 2 }; // + 1
	auto expandedMat = cv::Mat{
		{ mat.cols + mask.cols, mat.rows + mask.rows },
		mat.type(), cv::mean( mat )
	};
	mat.copyTo( expandedMat( { expandPoint, mat.size() } ) );

	for ( auto i = 0; i < expandedMat.cols - mask.cols; ++i ) {
		for ( auto j = 0; j < expandedMat.rows - mask.rows; ++j ) {
			auto value = compare< T1 >( expandedMat( { i, j, mask.cols, mask.rows } ), mask );
			if ( !chanceMap.empty() ) {
				value *= chanceMap.at< float >( { i, j } );
			}
			if ( value > max ) {
				max = value;
				maxPoint = { i, j };
				maxPoint -= expandPoint;
			}
		}
	}
}

//Find good rect for template
template < typename T1 >
auto	findTemplate( const cv::Mat_< T1 > & mat, const cv::Mat & mask, const cv::Mat & chanceMap = cv::Mat{} )
-> cv::Rect {
	if ( mat.empty() || mask.empty() ) {
		return{};
	}

	auto max = 0.0F;
	auto maxPoint = cv::Point{ -1, -1 };
	findGoodRect( mat, mask, maxPoint, max, chanceMap );

	return{ maxPoint, mask.size() };
}

//Find good rect for multisize template
template < typename T1 >
auto	fullFind( const cv::Mat_< T1 > & mat, const cv::Mat & mask )
-> cv::Rect {
	if ( mat.empty() || mask.empty() ) {
		return{};
	}

	auto goodMax = 0.0F;
	auto goodPoint = cv::Point{ 0, 0 };
	auto goodSize = cv::Size{ mask.size() };

	for ( auto i = mask.cols, j = mask.rows; i <= mat.cols && j <= mat.rows; ++i, ++j ) {
		auto max = 0.0F;
		auto maxPoint = cv::Point{ 0, 0 };
		auto newMask = cv::Mat{};
		cv::resize( mask, newMask, { i, j } );
		findGoodRect( mat, newMask, maxPoint, max );

		if ( max > goodMax ) {
			goodMax = max;
			goodPoint = maxPoint;
			goodSize = { i, j };
		}
	}

	return{ goodPoint, goodSize };
}

auto	detectEyeMask( const cv::Mat &in, int arg1 )->cv::Mat;

auto	getCenterMassOld( const cv::Mat & in )->cv::Point2f;

auto	randomColor()->cv::Scalar_<int>;

auto	resize( const cv::Mat & in, double k )->cv::Mat;
auto	filter( const cv::Mat &in )->cv::Mat;
auto	morph( const cv::Mat &in, int type, int arg = 5 )->cv::Mat;

void	initGPU();

auto	getSFlowPoint( const cv::Point & point, const cv::Mat & flow, int step )->cv::Point2d;
void	drawOptFlow( cv::Mat & mat, const cv::Mat & flow, int step, double k, const cv::Scalar & color );
