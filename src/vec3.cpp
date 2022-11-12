#include "vec3.h"

inline double vec3::length() const
{
	return sqrt(length_squared());
}

double vec3::length_squared() const
{
	return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

inline vec3 vec3::random()
{
	return vec3(random_double(), random_double(), random_double());
}

inline vec3 vec3::random(double min, double max)
{
	return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

double vec3::dot(const vec3 &u, const vec3 &v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

vec3 vec3::cross(const vec3 &u, const vec3 &v)
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

vec3 vec3::unit_vector(vec3 v)
{
	return v / v.length();
}

vec3 vec3::random_in_unit_sphere()
{
	auto p = vec3::random(-1, 1);
	return unit_vector(p);
}
vec3 vec3::random_in_hemisphere(const vec3 &normal)
{
	vec3 in_unit_sphere = vec3::random_in_unit_sphere();
	if (vec3::dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}