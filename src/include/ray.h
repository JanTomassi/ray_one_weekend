#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class ray {
public:
  ray() {}
  ray(const glm::vec3 &origin, const glm::vec3 &direction)
      : orig(origin), dir(direction) {}

  inline glm::dvec3 origin() const { return orig; }
  inline glm::dvec3 direction() const { return dir; }

  glm::dvec3 at(double t) const { return orig + t * dir; }

  glm::dvec3 orig;
  glm::dvec3 dir;
};

#endif
