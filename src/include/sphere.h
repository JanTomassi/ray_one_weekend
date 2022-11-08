#pragma once

#include "vec3.h"
#include "hittable.h"

class sphere : public hittable
{
public:
    sphere(){};
    sphere(cv::Vec3d cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m){};

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

public:
    cv::Vec3d center;
    double radius;
    shared_ptr<material> mat_ptr;
};