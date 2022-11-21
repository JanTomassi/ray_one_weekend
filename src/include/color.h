#pragma once

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

inline void changePixelColor(cv::Vec3d &pixel, glm::vec3 &colors) {
  auto r = colors[0];
  auto g = colors[1];
  auto b = colors[2];

  // Write the translated [0,255] value of each color component.
  pixel[2] = static_cast<double>(r);
  pixel[1] = static_cast<double>(g);
  pixel[0] = static_cast<double>(b);
}

inline void addPixelColor(cv::Vec3d &pixel, glm::vec3 &colors) {
  auto r = colors[0];
  auto g = colors[1];
  auto b = colors[2];

  // Write the translated [0,255] value of each color component.
  pixel[2] += static_cast<double>(r);
  pixel[1] += static_cast<double>(g);
  pixel[0] += static_cast<double>(b);
}
