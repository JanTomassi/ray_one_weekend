#include "triangle.h"

triangle::triangle(cv::Vec3f v0, cv::Vec3f v1, cv::Vec3f v2, shared_ptr<material> m) : mat_ptr(m)
{
    edge0 = v1 - v0;
    edge1 = v2 - v1;
    edge2 = v0 - v2;

    normal = edge0.cross(edge1);
}

bool triangle::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{
    // vec3 oc = r.orig - vec3(center[0], center[1], center[2]);
    // auto a = r.direction().length_squared();
    // auto half_b = vec3::dot(oc, r.direction());
    // auto c = oc.length_squared() - radius * radius;

    // auto discriminant = half_b * half_b - a * c;
    // if (discriminant < 0)
    //     return false;
    // auto sqrtd = sqrt(discriminant);

    // // Find the nearest root that lies in the acceptable range.
    // auto root = (-half_b - sqrtd) / a;
    // if (root < t_min || t_max < root)
    // {
    //     root = (-half_b + sqrtd) / a;
    //     if (root < t_min || t_max < root)
    //         return false;
    // }

    // rec.t = root;
    // rec.p = r.at(rec.t);
    // vec3 outward_normal = (rec.p - vec3(center[0], center[1], center[2])) / radius;
    // rec.set_face_normal(r, outward_normal);
    // rec.mat_ptr = mat_ptr;

        return true;
}