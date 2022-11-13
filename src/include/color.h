#pragma once

#include <opencv2/opencv.hpp>
#include "vec3.h"

void changePixelColor(cv::Vec3d &pixel, cv::Vec3d &colors)
{
	auto r = colors[0];
	auto g = colors[1];
	auto b = colors[2];

	// Write the translated [0,255] value of each color component.
	pixel[2] = static_cast<double>(r);
	pixel[1] = static_cast<double>(g);
	pixel[0] = static_cast<double>(b);
}

void addPixelColor(cv::Vec3d &pixel, cv::Vec3d &colors)
{
	auto r = colors[0];
	auto g = colors[1];
	auto b = colors[2];

	// Write the translated [0,255] value of each color component.
	pixel[2] += static_cast<double>(r);
	pixel[1] += static_cast<double>(g);
	pixel[0] += static_cast<double>(b);
}
