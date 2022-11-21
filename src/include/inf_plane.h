#pragma once

#include "hittable.h"
#include <opencv2/opencv.hpp>

class inf_plane : public hittable {
public:
  inf_plane(){};
  inf_plane(glm::vec3 cen, glm::vec3 n, std::shared_ptr<material> m)
      : center(cen), plane_normal(n), mat_ptr(m){};

  virtual bool hit(const ray &r, double t_min, double t_max,
                   hit_record &rec) const override;

public:
  glm::vec3 center;
  glm::vec3 plane_normal;
  std::shared_ptr<material> mat_ptr;
};

inline bool inf_plane::hit(const ray &r, double t_min, double t_max,
                           hit_record &rec) const {
  glm::vec3 ray_dir = glm::normalize(r.direction());
  glm::vec3 plane_vector = center + plane_normal;
  float numerator = glm::dot(center, plane_normal);
  float denumerator = glm::dot(ray_dir, plane_vector);
  float distance = numerator / denumerator;

  if (distance < t_min || t_max < distance) {
    return false;
  }

  rec.t = distance;
  rec.p = r.at(rec.t);
  // vec3 outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, plane_normal);
  rec.mat_ptr = mat_ptr;

  return true;
}
