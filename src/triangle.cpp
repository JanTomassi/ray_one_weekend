#include "triangle.h"
#include "vec3.h"

triangle::triangle(cv::Vec3f v0, cv::Vec3f v1, cv::Vec3f v2, shared_ptr<material> m) : v0(v0), v1(v1), v2(v2), mat_ptr(m)
{
    edge0 = v1 - v0;
    edge1 = v2 - v1;
    edge2 = v0 - v2;

    normal = edge0.cross(edge1);
}

bool triangle::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{

    // d = ((p_0 - l_0) * n) / (l * n)
    vec3 p_0 = vec3(v0(0), v0(1), v0(2));
    vec3 n = vec3(normal(0), normal(1), normal(2));

    rec.t = (vec3::dot(n, p_0 - r.origin()) / vec3::dot(n, r.direction()));

    if (rec.t < t_min || t_max < rec.t)
    {
        return false;
    }

    rec.p = r.at(rec.t);
    rec.set_face_normal(r, n);
    rec.mat_ptr = mat_ptr;

    return (vec3::dot(n, vec3::cross(vec3(edge0(0), edge0(1), edge0(2)), rec.p - vec3(v0(0), v0(1), v0(2)))) > 0 &&
            vec3::dot(n, vec3::cross(vec3(edge1(0), edge1(1), edge1(2)), rec.p - vec3(v1(0), v1(1), v1(2)))) > 0 &&
            vec3::dot(n, vec3::cross(vec3(edge2(0), edge2(1), edge2(2)), rec.p - vec3(v2(0), v2(1), v2(2)))) > 0);
}