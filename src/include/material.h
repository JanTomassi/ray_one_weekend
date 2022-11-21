#pragma once
#include "ray.h"
#include "rtweek.h"

struct hit_record;

class material {
public:
  virtual bool scatter(const ray &ray_in, const hit_record &hit_r,
                       glm::vec3 &attenuascion, ray &scattered) const = 0;
  virtual glm::vec3 emitted() const { return glm::vec3(0, 0, 0); }
};
