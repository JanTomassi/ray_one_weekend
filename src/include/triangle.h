#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "hittable.h"

class triangle : public hittable {
public:
  triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
           std::shared_ptr<material> m);

  virtual bool hit(const ray &r, double t_min, double t_max,
                   hit_record &rec) const override;

private:
  glm::dvec3 v0;
  glm::dvec3 v1;
  glm::dvec3 v2;

  glm::dvec3 edge0;
  glm::dvec3 edge1;
  glm::dvec3 edge2;

  glm::dvec3 normal;

  std::shared_ptr<material> mat_ptr;
};
