#pragma once
#include "material.h"
#include "hittable.h"

class defuse_light : virtual public material
{
public:
    defuse_light(const cv::Vec3d &a) : emit(a){};

    virtual bool scatter(const ray &ray_in, const hit_record &rec, cv::Vec3d &attenuation, ray &scattered) const override
    {
        return false;
    };

    virtual cv::Vec3d emitted() const { return emit; }

    cv::Vec3d emit;
};
