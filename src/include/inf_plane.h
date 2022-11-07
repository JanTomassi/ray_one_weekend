#pragma once

#include "vec3.h"
#include "hittable.h"

class inf_plane : public hittable {
public:
	inf_plane() {};
	inf_plane(point3 cen, vec3 n, shared_ptr<material> m) : center(cen), plane_normal(n), mat_ptr(m) {};

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
	point3 center;
	vec3 plane_normal;
	shared_ptr<material> mat_ptr;
};

bool inf_plane::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 ray_dir = vec3::unit_vector(r.direction());
	vec3 plane_vector = center + plane_normal;
	double numerator = vec3::dot(center, plane_vector);
	double denumerator = vec3::dot(ray_dir, plane_vector);
	double distance = numerator / denumerator;

	if (distance < t_min || t_max < distance) {
		return false;
	}

	rec.t = distance;
	rec.p = r.at(rec.t);
	//vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, plane_normal);
	rec.mat_ptr = mat_ptr;

	return true;
}
