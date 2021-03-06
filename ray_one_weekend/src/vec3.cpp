#include "vec3.h"



inline double vec3::length() const {
	return sqrt(length_squared());
}

inline double vec3::length_squared() const {
	return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

inline vec3 vec3::random() {
	return vec3(random_double(), random_double(), random_double());
}

inline vec3 vec3::random(double min, double max) {
	return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline double dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

vec3 random_in_unit_sphere() {
	while (true) {
		auto p = vec3::random(-1, 1);
		return unit_vector(p);
	}
}