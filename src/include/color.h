#pragma once

#include <opencv2/opencv.hpp>
#include "vec3.h"

void changePixelColor(cv::Vec3d &pixel, cv::Vec3d &colors)
{
	auto r = colors[0];
	auto g = colors[1];
	auto b = colors[2];	
	
	r = sqrt(r);
	g = sqrt(g);
	b = sqrt(b);

	// Write the translated [0,255] value of each color component.
	pixel[2] = static_cast<double>(clamp(r, 0.0, 0.999));
	pixel[1] = static_cast<double>(clamp(g, 0.0, 0.999));
	pixel[0] = static_cast<double>(clamp(b, 0.0, 0.999));
}

void addPixelColor(cv::Vec3d &pixel, cv::Vec3d &colors, int samples_per_pixel)
{
	auto r = colors[0];
	auto g = colors[1];
	auto b = colors[2];

	// Divide the color by the number of samples.
	r = sqrt(r);
	g = sqrt(g);
	b = sqrt(b);

	// Write the translated [0,255] value of each color component.
	pixel[2] += static_cast<double>(clamp(r, 0.0, 0.999));
	pixel[1] += static_cast<double>(clamp(g, 0.0, 0.999));
	pixel[0] += static_cast<double>(clamp(b, 0.0, 0.999));
}
