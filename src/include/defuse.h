#pragma once
#include "material.h"
#include "hittable.h"

class defuse : virtual public material
{
public:
	defuse(const cv::Vec3d &a) : albedo(a){};

	virtual bool scatter(const ray &ray_in, const hit_record &rec, cv::Vec3d &attenuation, ray &scattered) const override
	{
		auto scatter_direction = rec.normal + vec3::random_in_unit_sphere();
		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	};

	cv::Vec3d albedo;
};
