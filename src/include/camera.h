#pragma once
#include "rtweek.h"

class mycamera
{
public:
	mycamera()
	{
		auto viewport_height = 1.0;
		auto viewport_width = aspect_ratio * viewport_height;
		auto focal_length = 1.0;

		origin = point3(0, 0, 0);
	}

	mycamera(point3 orig)
	{
		auto viewport_height = 1.0;
		auto viewport_width = aspect_ratio * viewport_height;
		auto focal_length = 0.75;

		origin = orig;
	}

	ray get_ray(double u, double v)
	{
		cv::Mat1d dir = (intrinsicMatrix * cv::Vec3d(u, v, 1));
		return ray(origin, vec3(dir(0), dir(1), dir(2)));
	}

private:
	point3 origin;

	double m[3][3] = {{-1.0 / image_height, 0, (0.5 * (static_cast<double>(image_width) / image_height))},
					  {0, -1.0 / image_height, (0.5 * (static_cast<double>(image_height) / image_height))},
					  {0, 0, -1}};
	cv::Mat_<double> intrinsicMatrix = cv::Mat(3, 3, CV_64F, m);
};