#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "hittable.h"
#include "material.h"

class defuse : virtual public material {
public:
  glm::vec3 albedo;

  defuse(const glm::vec3 &a) : albedo(a){};

  glm::vec3 random_in_unite_sphere() const {
    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);

    float theta = 2 * glm::pi<double>() * distribution(mt);
    float phi = acos(1 - 1 * distribution(mt));
    float x = sin(phi) * cos(theta);
    float y = cos(phi);
    float z = sin(phi) * sin(theta);

    return glm::vec3(x, y, z);
  }

  virtual bool scatter(const ray &ray_in, const hit_record &rec,
                       glm::vec3 &attenuation, ray &scattered) const override {
    glm::vec3 scatter_direction = rec.normal + random_in_unite_sphere();
    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  };
};
