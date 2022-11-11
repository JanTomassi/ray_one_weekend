#pragma once

#include "vec3.h"
#include "hittable.h"
#include <opencv2/opencv.hpp>

class triangle : public hittable
{
public:
    triangle(cv::Vec3f v0, cv::Vec3f v1, cv::Vec3f v2, shared_ptr<material> m);

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

private:
    cv::Vec3f v0;
    cv::Vec3f v1;
    cv::Vec3f v2;

    cv::Vec3f edge0;
    cv::Vec3f edge1;
    cv::Vec3f edge2;

    cv::Vec3f normal;

    shared_ptr<material> mat_ptr;
};