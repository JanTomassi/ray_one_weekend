#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <glm/gtc/constants.hpp>
#include <limits>
#include <memory>
#include <random>

// Constants
const double infinity = std::numeric_limits<double>::infinity();

// Image
const auto aspect_ratio = 1 / 1;
const int image_height = 480;
const int image_width = static_cast<int>(image_height * aspect_ratio);
const int max_depth = 8;

// Utility Functions

inline double degrees_to_radians(float degrees) {
  return degrees * glm::pi<float>() / 180.0f;
}

inline double random_double() {
  static std::uniform_real_distribution<float> distribution(0.0, 1.0);
  static std::default_random_engine generator;
  return distribution(generator);
}

inline double random_double_gauss() {
  static std::normal_distribution<float> distribution(0, 0.5);
  static std::default_random_engine generator;
  return distribution(generator);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

#endif
