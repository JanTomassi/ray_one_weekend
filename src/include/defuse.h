#pragma once
#include "material.h"
#include "hittable.h"

class defuse : virtual public material
{
public:
	defuse(const cv::Vec3d &a) : albedo(a){};

	virtual bool scatter(const ray &ray_in, const hit_record &rec, cv::Vec3d &attenuation, ray &scattered) const override
	{
		auto to_light = vec3::unit_vector(vec3(0, 5, 0) - rec.p);
		auto scatter_direction = (vec3::random_in_hemisphere(rec.normal, to_light));
		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	};

	cv::Vec3d albedo;
};
