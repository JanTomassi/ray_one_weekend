#pragma once

#include <glm/glm.hpp>

#include "hittable.h"
#include "material.h"

class defuse_light : virtual public material {
public:
  defuse_light(const glm::vec3 &a) : emit(a){};

  virtual bool scatter(const ray &ray_in, const hit_record &rec,
                       glm::vec3 &attenuation, ray &scattered) const override {
    return false;
  };

  virtual glm::vec3 emitted() const override { return emit; }

  glm::vec3 emit;
};
