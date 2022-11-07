#pragma once
#include "material.h"
#include "hittable.h"

class defuse : virtual public material
{
public:
	defuse(const color& a) : albedo(a){};

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		auto scatter_direction = rec.normal + vec3::random_in_unit_sphere();
		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	};

	color albedo;
};

