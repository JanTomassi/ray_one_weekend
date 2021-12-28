#pragma once
#include "rtweek.h"

class mycamera
{
public:
	mycamera() {
		auto viewport_height = 1.0;
		auto viewport_width = aspect_ratio * viewport_height;
		auto focal_length = 1.0;

		origin = point3(0, 0, 0);
		horizontal = vec3(viewport_width, 0, 0);
		vertical = vec3(0, viewport_height, 0);
		upper_left_corner = origin - horizontal / 2 + vertical / 2 - vec3(0, 0, focal_length);
	}

	ray get_ray(double u, double v) {
		return ray(origin, upper_left_corner + u * horizontal - v * vertical);
	}

private:
	point3 origin;
	point3 upper_left_corner;
	vec3 horizontal;
	vec3 vertical;
};

