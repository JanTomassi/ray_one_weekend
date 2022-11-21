#pragma once
#include "ray.h"
#include "rtweek.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

class mycamera {
public:
  mycamera() { origin = glm::vec3(0, 0, 0); }

  mycamera(const glm::vec3 &orig, const glm::vec3 &lookAt) {
    origin = orig;
    cameraModel = glm::lookAt(origin, lookAt, glm::vec3(0, 1, 0));
  }

  ray get_ray(double u, double v) const {
    glm::vec3 dir = (glm::vec3(u, v, 1) * intrinsicMatrix);
    return ray(origin, glm::vec4(dir, 1) * cameraModel);
  }

private:
  glm::vec3 origin;
  glm::mat4x4 cameraModel;

  glm::mat3x3 intrinsicMatrix = glm::mat3x3(
      glm::vec3(-1.0 / image_height, 0,
                (0.5 * (static_cast<double>(image_width) / image_height))),
      glm::vec3(0, -1.0 / image_height,
                (0.5 * (static_cast<double>(image_height) / image_height))),
      glm::vec3(0, 0, -1));
};
