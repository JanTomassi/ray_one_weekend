#ifndef RAY_H
#define RAY_H

#include <opencv2/opencv.hpp>
#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction)
    {}

    inline point3 origin() const { return orig; }
    inline vec3 direction() const { return dir; }
    inline cv::Vec3d directionOPEN() const { return cv::Vec3d(dir.x(),dir.y(),dir.z()); }

    point3 at(double t) const {
        return orig + t * dir;
    }

    point3 orig;
    vec3 dir;
};

#endif