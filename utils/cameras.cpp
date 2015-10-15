#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include "qt.h"
#include "utils.h"
#include "cameras.h"

auto	getResolution( const cv::VideoCapture & capture ) -> cv::Size {
	return cv::Size2d{ capture.get( cv::CAP_PROP_FRAME_WIDTH ), capture.get( cv::CAP_PROP_FRAME_HEIGHT ) };
}

auto	setResolution( cv::VideoCapture & capture, const cv::Size & size ) -> cv::Size {
	capture.set( CV_CAP_PROP_FRAME_WIDTH, size.width );
	capture.set( CV_CAP_PROP_FRAME_HEIGHT, size.height );
	return getResolution( capture );
}

auto	setLowResolution( cv::VideoCapture & capture ) -> cv::Size {
	return setResolution( capture, { 320, 240 } );
}

auto	setMediumResolution( cv::VideoCapture & capture ) -> cv::Size {
	return setResolution( capture, { 640, 480 } );
}

auto	setHighResolution( cv::VideoCapture & capture ) -> cv::Size {
	return setResolution( capture, { 2000, 2000 } );
}

template < typename ... Args >
bool	isAvailability( Args && ... args ) {
	QCamera camera{ std::forward< Args >( args ) ... };
	return camera.availability() == QMultimedia::AvailabilityStatus::Available;
}

auto	getMaxResolutionCameraId() -> int {
	auto maxId = 0, maxSum = 0;
	auto i = 0;
	for ( auto && info : QCameraInfo::availableCameras() ) {
		if ( isAvailability( info ) ) {
			cv::VideoCapture camera{ i };
			if ( camera.isOpened() ) {
				auto size = setHighResolution( camera );
				auto sum = size.width + size.height;
				if ( sum > maxSum ) {
					maxSum = sum;
					maxId = i;
				}
			}
		}

		++i;
	}

	return maxId;
}

void	showAllCameras() {
	try {
		auto i = 0;
		static auto initialization = false;
		static std::vector< cv::VideoCapture > cameras;
		for ( auto && info : QCameraInfo::availableCameras() ) {
			QCamera qtCamera{ info };
			auto availability = qtCamera.availability() == QMultimedia::AvailabilityStatus::Available;
			qDebug()
				<< "Camera %1: "_q.arg( i + 1 ).toStdString().c_str() << info.description() << endl
				<< "Device name: " << info.deviceName() << endl
				<< "Position: " << info.position() << endl
				<< "Orientation: " << info.orientation() << endl
				<< "Is default: " << ( info == QCameraInfo::defaultCamera() ) << endl
				<< "Availability: " << availability << endl;
			if ( cameras.size() < i + 1 ) {
				cameras.emplace_back( i );
			}

			cv::VideoCapture camera{ cameras[ i ] };
			if ( !initialization && camera.isOpened() && availability ) {
				setHighResolution( camera );
			}

			if ( camera.isOpened() ) {
				cv::Mat frame;
				camera >> frame;
				imshow_s( info.description().toStdString(), frame );
				qDebug()
					<< "Resolution: " << camera.get( cv::CAP_PROP_FRAME_WIDTH ) << "x" << camera.get( cv::CAP_PROP_FRAME_HEIGHT ) << endl
					<< "Brightness: " << camera.get( cv::CAP_PROP_BRIGHTNESS ) << endl
					<< "Contrast: " << camera.get( cv::CAP_PROP_CONTRAST ) << endl
					<< "Saturation: " << camera.get( cv::CAP_PROP_SATURATION ) << endl
					<< "Hue: " << camera.get( cv::CAP_PROP_HUE ) << endl
					<< "Gain: " << camera.get( cv::CAP_PROP_GAIN ) << endl
					<< "Exposure: " << camera.get( cv::CAP_PROP_EXPOSURE ) << endl
					<< "Convert to RGB: " << camera.get( cv::CAP_PROP_CONVERT_RGB ) << endl;
			}
			i++;
		}
		initialization = true;
		/*
		QCamera camera;
		camera.load();
		QCameraImageCapture imageCapture{ &camera };

		QObject::connect( &camera, &QCamera::statusChanged, [ & ] ( QCamera::Status in ) {
		qDebug() << "New state: " << in;

		for ( const auto & size : imageCapture.supportedResolutions() ) {
		qDebug() << "Size: " << size;
		}
		} );
		*/
	} catch ( const cv::Exception & exception ) {
		qDebug() << exception.what();
	}
}
