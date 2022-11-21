
#include "triangle.h"

triangle::triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
                   std::shared_ptr<material> m)
    : v0(v0), v1(v1), v2(v2), mat_ptr(m) {
  edge0 = v1 - v0;
  edge1 = v2 - v1;
  edge2 = v0 - v2;

  normal = glm::cross(edge0, edge1);
}

bool triangle::hit(const ray &r, double t_min, double t_max,
                   hit_record &rec) const {

  // d = ((p_0 - l_0) * n) / (l * n)
  glm::dvec3 p_0 = v0;
  glm::dvec3 n = normal;

  rec.t = (glm::dot(n, p_0 - r.origin()) / glm::dot(n, r.direction()));

  if (rec.t < t_min || t_max < rec.t) {
    return false;
  }

  rec.p = r.at(rec.t);
  rec.set_face_normal(r, n);
  rec.mat_ptr = mat_ptr;

  return (glm::dot(n, glm::cross(edge0, rec.p - v0)) > 0 &&
          glm::dot(n, glm::cross(edge1, rec.p - v1)) > 0 &&
          glm::dot(n, glm::cross(edge2, rec.p - v2)) > 0);
}
