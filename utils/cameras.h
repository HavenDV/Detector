#pragma once
#include <opencv2\opencv.hpp>

auto	getResolution( const cv::VideoCapture & capture ) -> cv::Size;
auto	setResolution( cv::VideoCapture & capture, const cv::Size & size ) -> cv::Size;
auto	setLowResolution( cv::VideoCapture & capture ) -> cv::Size;
auto	setMediumResolution( cv::VideoCapture & capture ) -> cv::Size;
auto	setHighResolution( cv::VideoCapture & capture ) -> cv::Size;

auto	getMaxResolutionCameraId() -> int;
void	showAllCameras();