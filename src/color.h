#pragma once

#include<opencv2/opencv.hpp>
#include "vec3.h"
using namespace std;

void changePixelColor(cv::Vec3d& pixel, color colors, int samples_per_pixel) {
	auto r = colors.x();
	auto g = colors.y();
	auto b = colors.z();

	// Divide the color by the number of samples.
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale*r);
	g = sqrt(scale*g);
	b = sqrt(scale*b);

	// Write the translated [0,255] value of each color component.
	pixel[2] = static_cast<double>(clamp(r, 0.0, 0.999));
	pixel[1] = static_cast<double>(clamp(g, 0.0, 0.999));
	pixel[0] = static_cast<double>(clamp(b, 0.0, 0.999));
}

void addPixelColor(cv::Vec3d& pixel, color colors, int samples_per_pixel) {
	auto r = colors.x();
	auto g = colors.y();
	auto b = colors.z();

	// Divide the color by the number of samples.
	r = sqrt(r);
	g = sqrt(g);
	b = sqrt(b);

	// Write the translated [0,255] value of each color component.
	pixel[2] += static_cast<double>(clamp(r, 0.0, 0.999));
	pixel[1] += static_cast<double>(clamp(g, 0.0, 0.999));
	pixel[0] += static_cast<double>(clamp(b, 0.0, 0.999));
}
