#pragma once
#include "rtweek.h"

struct hit_record;

class material
{
public:
	virtual bool scatter(const ray &ray_in, const hit_record &hit_r, cv::Vec3d &attenuascion, ray &scattered) const = 0;
};

#include "defuse.h"