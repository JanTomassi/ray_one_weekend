#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

#include "./camera.h"
#include "./color.h"
#include "./defuse.h"
#include "./defuse_light.h"
#include "./hittable_list.h"
#include "./material.h"
#include "./ray.h"
#include "./rtweek.h"

#include "./inf_plane.h"
#include "./sphere.h"
#include "./triangle.h"

void cWindow(cv::String windowName);
void render();
void renderLoop(int name);
void dWindow(cv::String windowName);
void addTeaPot(shared_ptr<defuse> tea_pot);

cv::String windowName = "Ray Tracing";
cv::Mat img(image_height, image_width, CV_64FC3, cv::Scalar(0, 0, 0));
mycamera cam(glm::vec3(0, 4, 4), glm::vec3(0, 1, 0));
hittable_list world;

int thread;
int samples_per_pixel;
int printN;

inline glm::vec3 ray_color(const ray &r, const hittable &world, int depth) {
  hit_record rec;

  if (depth <= 0)
    return glm::vec3(0, 0, 0);

  if (!world.hit(r, 0.0001, infinity, rec))
    return glm::vec3(0, 0, 0);

  ray scattered;
  glm::vec3 attenuation;
  glm::vec3 emitted = rec.mat_ptr->emitted();

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted + attenuation * (ray_color(scattered, world, depth - 1));

  // cv::Vec3d unit_direction = cv::normalize(r.directionOPEN());
  // auto hight = 0.5 * (unit_direction[1] + 1.0);
  // return (1.0 - hight) * cv::Vec3d(1.0, 1.0, 1.0) + hight * cv::Vec3d(0.5,
  // 0.7, 1.0);
}

int main(int argc, char **argv) {
  thread = std::atoi(argv[1]);
  printN = std::atoi(argv[3]);
  samples_per_pixel = std::atoi(argv[2]);
  cWindow(windowName);

  render();

  // cv::imshow(windowName, img / (samples_per_pixel * thread)); // Show our
  // image inside the created window.

  img.convertTo(img, CV_8U,
                255 / (static_cast<float>(samples_per_pixel) * thread), 0);

  cv::Mat lookUpTable(1, 256, CV_8U);
  uchar *p = lookUpTable.ptr();
  for (int i = 0; i < 256; ++i)
    p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, 0.5) * 255.0);
  cv::Mat res = img.clone();
  LUT(img, lookUpTable, res);

  auto start =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  auto imgPath = std::stringstream();
  imgPath << "render-img-" << ctime(&start) << ".png";

  auto resPath = std::stringstream();
  resPath << "render-res-" << ctime(&start) << ".png";

  cv::imwrite(cv::String(imgPath.str()), img);
  cv::imwrite(cv::String(resPath.str()), res);
  cv::imwrite("render-res.png", res);

  // dWindow(windowName);
  return 0;
}

void render() {

  // Texture
  auto material_ground = std::make_shared<defuse>(glm::vec3(0.1, 0.1, 0.1));
  auto material_simple = std::make_shared<defuse>(glm::vec3(1, 0.4, 0.4));
  auto material_light =
      std::make_shared<defuse_light>(glm::vec3(1, 1, 1) * 2.0f);
  auto material_tea_pot = std::make_shared<defuse>(glm::vec3(0.47, 0.27, 0.08));

  // Object
  world.add(std::make_shared<sphere>(glm::vec3(0, -1000.05, 0), 1000,
                                     material_ground));

  world.add(std::make_shared<sphere>(glm::vec3(0, 3, 0), 1, material_light));

  addTeaPot(material_tea_pot);

  if (thread) {
    std::vector<std::thread> workingThread;
    for (int i = 1; i <= thread; i++) {
      workingThread.push_back(std::thread(renderLoop, i));
    }
    for (int i = 0; i < thread; i++) {
      workingThread[i].join();
    }
  } else {
    std::thread t1(renderLoop, 1);
    t1.join();
  }
}

std::mutex m_image;
void renderLoop(int name) {
  thread_local cv::Mat t_img(image_height, image_width, CV_64FC3,
                             cv::Scalar(0, 0, 0));
  thread_local int sampleN = 0;

  while (sampleN++ < samples_per_pixel) {
    for (int i = 0; i < image_height; ++i) {
      for (int j = 0; j < image_width; ++j) {
        auto u = (j + random_double(-1, 1));
        auto v = (i + random_double(-1, 1));
        ray r = cam.get_ray(u, v);

        glm::vec3 pixel_color = ray_color(r, world, max_depth);
        cv::Vec3d &point = t_img.at<cv::Vec3d>(i, j);

<<<<<<< HEAD
        addPixelColor(point, pixel_color);
      }
    }
    if (!(sampleN % printN)) {
      m_image.lock();
      std::cout << "\nRemaining time for " << name << ": \t" << sampleN << '/'
                << samples_per_pixel << "\t" << std::flush;
      img += t_img;
      cv::imshow(windowName, img / (sampleN * thread));
      cv::waitKey(10);
      m_image.unlock();
    }
  }
=======
        addPixelColor(point, pixel_color);
      }
    }
    if (!(sampleN % printN)) {
      std::cout << "\nRemaining time for " << name << ": " << sampleN << '/'
                << samples_per_pixel << "\t" << std::flush;
      m_image.lock();
      img += t_img;
      cv::imshow(windowName,
                 img / (sampleN *
                        thread)); // Show our image inside the created window.
      cv::waitKey(5);
      m_image.unlock();
    }
  }
>>>>>>> PointLight
}

void cWindow(cv::String windowName) {
  // Create a window
  namedWindow(windowName, cv::WINDOW_NORMAL);
}

void dWindow(cv::String windowName) {
  while (cv::waitKey(0) != ' ') {
  }
  // destroy the created window
  cv::destroyWindow(windowName);
}

void addTeaPot(shared_ptr<defuse> tea_pot) {
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.7, 1.2, 0.0), glm::dvec3(0.6056, 1.2, -0.3557),
      glm::dvec3(0.5988, 1.2437, -0.3517), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.5988, 1.2437, -0.3517),
                                       glm::dvec3(0.6921, 1.2437, 0.0),
                                       glm::dvec3(0.7, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6921, 1.2437, 0.0), glm::dvec3(0.5988, 1.2437, -0.3517),
      glm::dvec3(0.6196, 1.2437, -0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6196, 1.2437, -0.3639), glm::dvec3(0.7162, 1.2437, 0.0),
      glm::dvec3(0.6921, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.7162, 1.2437, 0.0), glm::dvec3(0.6196, 1.2437, -0.3639),
      glm::dvec3(0.6489, 1.2, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 1.2, -0.3811), glm::dvec3(0.75, 1.2, 0.0),
      glm::dvec3(0.7162, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6056, 1.2, -0.3557), glm::dvec3(0.3557, 1.2, -0.6056),
      glm::dvec3(0.3517, 1.2437, -0.5988), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3517, 1.2437, -0.5988), glm::dvec3(0.5988, 1.2437, -0.3517),
      glm::dvec3(0.6056, 1.2, -0.3557), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5988, 1.2437, -0.3517), glm::dvec3(0.3517, 1.2437, -0.5988),
      glm::dvec3(0.3639, 1.2438, -0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3639, 1.2438, -0.6196), glm::dvec3(0.6196, 1.2437, -0.3639),
      glm::dvec3(0.5988, 1.2437, -0.3517), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6196, 1.2437, -0.3639), glm::dvec3(0.3639, 1.2438, -0.6196),
      glm::dvec3(0.3811, 1.2, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 1.2, -0.6489), glm::dvec3(0.6489, 1.2, -0.3811),
      glm::dvec3(0.6196, 1.2437, -0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3557, 1.2, -0.6056), glm::dvec3(0.0, 1.2, -0.7),
      glm::dvec3(0.0, 1.2437, -0.6921), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2437, -0.6921), glm::dvec3(0.3517, 1.2437, -0.5988),
      glm::dvec3(0.3557, 1.2, -0.6056), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3517, 1.2437, -0.5988), glm::dvec3(0.0, 1.2437, -0.6921),
      glm::dvec3(0.0, 1.2438, -0.7162), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2438, -0.7162), glm::dvec3(0.3639, 1.2438, -0.6196),
      glm::dvec3(0.3517, 1.2437, -0.5988), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.3639, 1.2438, -0.6196),
                                       glm::dvec3(0.0, 1.2438, -0.7162),
                                       glm::dvec3(0.0, 1.2, -0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, -0.75), glm::dvec3(0.3811, 1.2, -0.6489),
      glm::dvec3(0.3639, 1.2438, -0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, -0.7), glm::dvec3(-0.3757, 1.2, -0.6056),
      glm::dvec3(-0.3576, 1.2437, -0.5988), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.3576, 1.2437, -0.5988),
                                       glm::dvec3(0.0, 1.2437, -0.6921),
                                       glm::dvec3(0.0, 1.2, -0.7), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2437, -0.6921), glm::dvec3(-0.3576, 1.2437, -0.5988),
      glm::dvec3(-0.3647, 1.2437, -0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3647, 1.2437, -0.6196), glm::dvec3(0.0, 1.2438, -0.7162),
      glm::dvec3(0.0, 1.2437, -0.6921), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2438, -0.7162), glm::dvec3(-0.3647, 1.2437, -0.6196),
      glm::dvec3(-0.3811, 1.2, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 1.2, -0.6489), glm::dvec3(0.0, 1.2, -0.75),
      glm::dvec3(0.0, 1.2438, -0.7162), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3757, 1.2, -0.6056), glm::dvec3(-0.6156, 1.2, -0.3557),
      glm::dvec3(-0.6018, 1.2437, -0.3517), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.6018, 1.2437, -0.3517),
                                       glm::dvec3(-0.3576, 1.2437, -0.5988),
                                       glm::dvec3(-0.3757, 1.2, -0.6056),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.3576, 1.2437, -0.5988),
                                       glm::dvec3(-0.6018, 1.2437, -0.3517),
                                       glm::dvec3(-0.62, 1.2437, -0.3639),
                                       tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.62, 1.2437, -0.3639), glm::dvec3(-0.3647, 1.2437, -0.6196),
      glm::dvec3(-0.3576, 1.2437, -0.5988), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3647, 1.2437, -0.6196), glm::dvec3(-0.62, 1.2437, -0.3639),
      glm::dvec3(-0.6489, 1.2, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 1.2, -0.3811), glm::dvec3(-0.3811, 1.2, -0.6489),
      glm::dvec3(-0.3647, 1.2437, -0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6156, 1.2, -0.3557), glm::dvec3(-0.7, 1.2, 0.0),
      glm::dvec3(-0.6921, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6921, 1.2437, 0.0), glm::dvec3(-0.6018, 1.2437, -0.3517),
      glm::dvec3(-0.6156, 1.2, -0.3557), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6018, 1.2437, -0.3517), glm::dvec3(-0.6921, 1.2437, 0.0),
      glm::dvec3(-0.7162, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.7162, 1.2437, 0.0), glm::dvec3(-0.62, 1.2437, -0.3639),
      glm::dvec3(-0.6018, 1.2437, -0.3517), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.62, 1.2437, -0.3639),
                                       glm::dvec3(-0.7162, 1.2437, 0.0),
                                       glm::dvec3(-0.75, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.75, 1.2, 0.0), glm::dvec3(-0.6489, 1.2, -0.3811),
      glm::dvec3(-0.62, 1.2437, -0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.7, 1.2, 0.0), glm::dvec3(-0.6056, 1.2, 0.3557),
      glm::dvec3(-0.5988, 1.2437, 0.3517), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.5988, 1.2437, 0.3517),
                                       glm::dvec3(-0.6921, 1.2437, 0.0),
                                       glm::dvec3(-0.7, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6921, 1.2437, 0.0), glm::dvec3(-0.5988, 1.2437, 0.3517),
      glm::dvec3(-0.6196, 1.2437, 0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6196, 1.2437, 0.3639), glm::dvec3(-0.7162, 1.2437, 0.0),
      glm::dvec3(-0.6921, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.7162, 1.2437, 0.0), glm::dvec3(-0.6196, 1.2437, 0.3639),
      glm::dvec3(-0.6489, 1.2, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 1.2, 0.3811), glm::dvec3(-0.75, 1.2, 0.0),
      glm::dvec3(-0.7162, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6056, 1.2, 0.3557), glm::dvec3(-0.3557, 1.2, 0.6056),
      glm::dvec3(-0.3517, 1.2437, 0.5988), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3517, 1.2437, 0.5988), glm::dvec3(-0.5988, 1.2437, 0.3517),
      glm::dvec3(-0.6056, 1.2, 0.3557), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5988, 1.2437, 0.3517), glm::dvec3(-0.3517, 1.2437, 0.5988),
      glm::dvec3(-0.3639, 1.2437, 0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3639, 1.2437, 0.6196), glm::dvec3(-0.6196, 1.2437, 0.3639),
      glm::dvec3(-0.5988, 1.2437, 0.3517), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6196, 1.2437, 0.3639), glm::dvec3(-0.3639, 1.2437, 0.6196),
      glm::dvec3(-0.3811, 1.2, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 1.2, 0.6489), glm::dvec3(-0.6489, 1.2, 0.3811),
      glm::dvec3(-0.6196, 1.2437, 0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3557, 1.2, 0.6056), glm::dvec3(0.0, 1.2, 0.7),
      glm::dvec3(0.0, 1.2437, 0.6921), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2437, 0.6921), glm::dvec3(-0.3517, 1.2437, 0.5988),
      glm::dvec3(-0.3557, 1.2, 0.6056), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3517, 1.2437, 0.5988), glm::dvec3(0.0, 1.2437, 0.6921),
      glm::dvec3(0.0, 1.2437, 0.7162), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2437, 0.7162), glm::dvec3(-0.3639, 1.2437, 0.6196),
      glm::dvec3(-0.3517, 1.2437, 0.5988), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.3639, 1.2437, 0.6196),
                                       glm::dvec3(0.0, 1.2437, 0.7162),
                                       glm::dvec3(0.0, 1.2, 0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, 0.75), glm::dvec3(-0.3811, 1.2, 0.6489),
      glm::dvec3(-0.3639, 1.2437, 0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, 0.7), glm::dvec3(0.3557, 1.2, 0.6056),
      glm::dvec3(0.3517, 1.2437, 0.5988), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.3517, 1.2437, 0.5988),
                                       glm::dvec3(0.0, 1.2437, 0.6921),
                                       glm::dvec3(0.0, 1.2, 0.7), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2437, 0.6921), glm::dvec3(0.3517, 1.2437, 0.5988),
      glm::dvec3(0.3639, 1.2437, 0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3639, 1.2437, 0.6196), glm::dvec3(0.0, 1.2437, 0.7162),
      glm::dvec3(0.0, 1.2437, 0.6921), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2437, 0.7162), glm::dvec3(0.3639, 1.2437, 0.6196),
      glm::dvec3(0.3811, 1.2, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 1.2, 0.6489), glm::dvec3(0.0, 1.2, 0.75),
      glm::dvec3(0.0, 1.2437, 0.7162), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3557, 1.2, 0.6056), glm::dvec3(0.6056, 1.2, 0.3557),
      glm::dvec3(0.5988, 1.2437, 0.3517), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5988, 1.2437, 0.3517), glm::dvec3(0.3517, 1.2437, 0.5988),
      glm::dvec3(0.3557, 1.2, 0.6056), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3517, 1.2437, 0.5988), glm::dvec3(0.5988, 1.2437, 0.3517),
      glm::dvec3(0.6196, 1.2437, 0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6196, 1.2437, 0.3639), glm::dvec3(0.3639, 1.2437, 0.6196),
      glm::dvec3(0.3517, 1.2437, 0.5988), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3639, 1.2437, 0.6196), glm::dvec3(0.6196, 1.2437, 0.3639),
      glm::dvec3(0.6489, 1.2, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 1.2, 0.3811), glm::dvec3(0.3811, 1.2, 0.6489),
      glm::dvec3(0.3639, 1.2437, 0.6196), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6056, 1.2, 0.3557), glm::dvec3(0.7, 1.2, 0.0),
      glm::dvec3(0.6921, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6921, 1.2437, 0.0), glm::dvec3(0.5988, 1.2437, 0.3517),
      glm::dvec3(0.6056, 1.2, 0.3557), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5988, 1.2437, 0.3517), glm::dvec3(0.6921, 1.2437, 0.0),
      glm::dvec3(0.7162, 1.2437, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.7162, 1.2437, 0.0), glm::dvec3(0.6196, 1.2437, 0.3639),
      glm::dvec3(0.5988, 1.2437, 0.3517), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.6196, 1.2437, 0.3639),
                                       glm::dvec3(0.7162, 1.2437, 0.0),
                                       glm::dvec3(0.75, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.75, 1.2, 0.0), glm::dvec3(0.6489, 1.2, 0.3811),
      glm::dvec3(0.6196, 1.2437, 0.3639), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.75, 1.2, 0.0), glm::dvec3(0.6489, 1.2, -0.3811),
      glm::dvec3(0.753, 0.9389, -0.4423), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.753, 0.9389, -0.4423),
                                       glm::dvec3(0.8704, 0.9389, 0.0),
                                       glm::dvec3(0.75, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8704, 0.9389, 0.0), glm::dvec3(0.753, 0.9389, -0.4423),
      glm::dvec3(0.8331, 0.6861, -0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8331, 0.6861, -0.4893), glm::dvec3(0.963, 0.6861, 0.0),
      glm::dvec3(0.8704, 0.9389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.963, 0.6861, 0.0), glm::dvec3(0.8331, 0.6861, -0.4893),
      glm::dvec3(0.8652, 0.45, -0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8652, 0.45, -0.5081), glm::dvec3(1.0, 0.45, 0.0),
      glm::dvec3(0.963, 0.6861, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 1.2, -0.3811), glm::dvec3(0.3811, 1.2, -0.6489),
      glm::dvec3(0.4423, 0.9389, -0.753), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4423, 0.9389, -0.753), glm::dvec3(0.753, 0.9389, -0.4423),
      glm::dvec3(0.6489, 1.2, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.753, 0.9389, -0.4423), glm::dvec3(0.4423, 0.9389, -0.753),
      glm::dvec3(0.4893, 0.6861, -0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4893, 0.6861, -0.8331), glm::dvec3(0.8331, 0.6861, -0.4893),
      glm::dvec3(0.753, 0.9389, -0.4423), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8331, 0.6861, -0.4893), glm::dvec3(0.4893, 0.6861, -0.8331),
      glm::dvec3(0.5081, 0.45, -0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5081, 0.45, -0.8652), glm::dvec3(0.8652, 0.45, -0.5081),
      glm::dvec3(0.8331, 0.6861, -0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 1.2, -0.6489), glm::dvec3(0.0, 1.2, -0.75),
      glm::dvec3(0.0, 0.9389, -0.8704), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.9389, -0.8704), glm::dvec3(0.4423, 0.9389, -0.753),
      glm::dvec3(0.3811, 1.2, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4423, 0.9389, -0.753), glm::dvec3(0.0, 0.9389, -0.8704),
      glm::dvec3(0.0, 0.6861, -0.963), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.6861, -0.963), glm::dvec3(0.4893, 0.6861, -0.8331),
      glm::dvec3(0.4423, 0.9389, -0.753), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.4893, 0.6861, -0.8331),
                                       glm::dvec3(0.0, 0.6861, -0.963),
                                       glm::dvec3(0.0, 0.45, -1.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.45, -1.0), glm::dvec3(0.5081, 0.45, -0.8652),
      glm::dvec3(0.4893, 0.6861, -0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, -0.75), glm::dvec3(-0.3811, 1.2, -0.6489),
      glm::dvec3(-0.4423, 0.9389, -0.753), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4423, 0.9389, -0.753),
                                       glm::dvec3(0.0, 0.9389, -0.8704),
                                       glm::dvec3(0.0, 1.2, -0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.9389, -0.8704), glm::dvec3(-0.4423, 0.9389, -0.753),
      glm::dvec3(-0.4893, 0.6861, -0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4893, 0.6861, -0.8331), glm::dvec3(0.0, 0.6861, -0.963),
      glm::dvec3(0.0, 0.9389, -0.8704), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.6861, -0.963), glm::dvec3(-0.4893, 0.6861, -0.8331),
      glm::dvec3(-0.5081, 0.45, -0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5081, 0.45, -0.8652), glm::dvec3(0.0, 0.45, -1.0),
      glm::dvec3(0.0, 0.6861, -0.963), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 1.2, -0.6489), glm::dvec3(-0.6489, 1.2, -0.3811),
      glm::dvec3(-0.753, 0.9389, -0.4423), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.753, 0.9389, -0.4423), glm::dvec3(-0.4423, 0.9389, -0.753),
      glm::dvec3(-0.3811, 1.2, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4423, 0.9389, -0.753), glm::dvec3(-0.753, 0.9389, -0.4423),
      glm::dvec3(-0.8331, 0.6861, -0.4893), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.8331, 0.6861, -0.4893),
                                       glm::dvec3(-0.4893, 0.6861, -0.8331),
                                       glm::dvec3(-0.4423, 0.9389, -0.753),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4893, 0.6861, -0.8331),
                                       glm::dvec3(-0.8331, 0.6861, -0.4893),
                                       glm::dvec3(-0.8652, 0.45, -0.5081),
                                       tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8652, 0.45, -0.5081), glm::dvec3(-0.5081, 0.45, -0.8652),
      glm::dvec3(-0.4893, 0.6861, -0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 1.2, -0.3811), glm::dvec3(-0.75, 1.2, 0.0),
      glm::dvec3(-0.8704, 0.9389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8704, 0.9389, 0.0), glm::dvec3(-0.753, 0.9389, -0.4423),
      glm::dvec3(-0.6489, 1.2, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.753, 0.9389, -0.4423), glm::dvec3(-0.8704, 0.9389, 0.0),
      glm::dvec3(-0.963, 0.6861, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.963, 0.6861, 0.0), glm::dvec3(-0.8331, 0.6861, -0.4893),
      glm::dvec3(-0.753, 0.9389, -0.4423), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.8331, 0.6861, -0.4893),
                                       glm::dvec3(-0.963, 0.6861, 0.0),
                                       glm::dvec3(-1.0, 0.45, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.0, 0.45, 0.0), glm::dvec3(-0.8652, 0.45, -0.5081),
      glm::dvec3(-0.8331, 0.6861, -0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.75, 1.2, 0.0), glm::dvec3(-0.6489, 1.2, 0.3811),
      glm::dvec3(-0.753, 0.9389, 0.4423), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.753, 0.9389, 0.4423),
                                       glm::dvec3(-0.8704, 0.9389, 0.0),
                                       glm::dvec3(-0.75, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8704, 0.9389, 0.0), glm::dvec3(-0.753, 0.9389, 0.4423),
      glm::dvec3(-0.8331, 0.6861, 0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8331, 0.6861, 0.4893), glm::dvec3(-0.963, 0.6861, 0.0),
      glm::dvec3(-0.8704, 0.9389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.963, 0.6861, 0.0), glm::dvec3(-0.8331, 0.6861, 0.4893),
      glm::dvec3(-0.8652, 0.45, 0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8652, 0.45, 0.5081), glm::dvec3(-1.0, 0.45, 0.0),
      glm::dvec3(-0.963, 0.6861, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 1.2, 0.3811), glm::dvec3(-0.3811, 1.2, 0.6489),
      glm::dvec3(-0.4423, 0.9389, 0.753), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4423, 0.9389, 0.753), glm::dvec3(-0.753, 0.9389, 0.4423),
      glm::dvec3(-0.6489, 1.2, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.753, 0.9389, 0.4423), glm::dvec3(-0.4423, 0.9389, 0.753),
      glm::dvec3(-0.4893, 0.6861, 0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4893, 0.6861, 0.8331), glm::dvec3(-0.8331, 0.6861, 0.4893),
      glm::dvec3(-0.753, 0.9389, 0.4423), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8331, 0.6861, 0.4893), glm::dvec3(-0.4893, 0.6861, 0.8331),
      glm::dvec3(-0.5081, 0.45, 0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5081, 0.45, 0.8652), glm::dvec3(-0.8652, 0.45, 0.5081),
      glm::dvec3(-0.8331, 0.6861, 0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 1.2, 0.6489), glm::dvec3(0.0, 1.2, 0.75),
      glm::dvec3(0.0, 0.9389, 0.8704), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.9389, 0.8704), glm::dvec3(-0.4423, 0.9389, 0.753),
      glm::dvec3(-0.3811, 1.2, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4423, 0.9389, 0.753), glm::dvec3(0.0, 0.9389, 0.8704),
      glm::dvec3(0.0, 0.6861, 0.963), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.6861, 0.963), glm::dvec3(-0.4893, 0.6861, 0.8331),
      glm::dvec3(-0.4423, 0.9389, 0.753), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4893, 0.6861, 0.8331),
                                       glm::dvec3(0.0, 0.6861, 0.963),
                                       glm::dvec3(0.0, 0.45, 1.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.45, 1.0), glm::dvec3(-0.5081, 0.45, 0.8652),
      glm::dvec3(-0.4893, 0.6861, 0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, 0.75), glm::dvec3(0.3811, 1.2, 0.6489),
      glm::dvec3(0.4423, 0.9389, 0.753), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.4423, 0.9389, 0.753),
                                       glm::dvec3(0.0, 0.9389, 0.8704),
                                       glm::dvec3(0.0, 1.2, 0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.9389, 0.8704), glm::dvec3(0.4423, 0.9389, 0.753),
      glm::dvec3(0.4893, 0.6861, 0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4893, 0.6861, 0.8331), glm::dvec3(0.0, 0.6861, 0.963),
      glm::dvec3(0.0, 0.9389, 0.8704), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.6861, 0.963), glm::dvec3(0.4893, 0.6861, 0.8331),
      glm::dvec3(0.5081, 0.45, 0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5081, 0.45, 0.8652), glm::dvec3(0.0, 0.45, 1.0),
      glm::dvec3(0.0, 0.6861, 0.963), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 1.2, 0.6489), glm::dvec3(0.6489, 1.2, 0.3811),
      glm::dvec3(0.753, 0.9389, 0.4423), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.753, 0.9389, 0.4423), glm::dvec3(0.4423, 0.9389, 0.753),
      glm::dvec3(0.3811, 1.2, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4423, 0.9389, 0.753), glm::dvec3(0.753, 0.9389, 0.4423),
      glm::dvec3(0.8331, 0.6861, 0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8331, 0.6861, 0.4893), glm::dvec3(0.4893, 0.6861, 0.8331),
      glm::dvec3(0.4423, 0.9389, 0.753), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4893, 0.6861, 0.8331), glm::dvec3(0.8331, 0.6861, 0.4893),
      glm::dvec3(0.8652, 0.45, 0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8652, 0.45, 0.5081), glm::dvec3(0.5081, 0.45, 0.8652),
      glm::dvec3(0.4893, 0.6861, 0.8331), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 1.2, 0.3811), glm::dvec3(0.75, 1.2, 0.0),
      glm::dvec3(0.8704, 0.9389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8704, 0.9389, 0.0), glm::dvec3(0.753, 0.9389, 0.4423),
      glm::dvec3(0.6489, 1.2, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.753, 0.9389, 0.4423), glm::dvec3(0.8704, 0.9389, 0.0),
      glm::dvec3(0.963, 0.6861, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.963, 0.6861, 0.0), glm::dvec3(0.8331, 0.6861, 0.4893),
      glm::dvec3(0.753, 0.9389, 0.4423), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.8331, 0.6861, 0.4893),
                                       glm::dvec3(0.963, 0.6861, 0.0),
                                       glm::dvec3(1.0, 0.45, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.0, 0.45, 0.0), glm::dvec3(0.8652, 0.45, 0.5081),
      glm::dvec3(0.8331, 0.6861, 0.4893), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.0, 0.45, 0.0), glm::dvec3(0.8652, 0.45, -0.5081),
      glm::dvec3(0.8091, 0.2611, -0.4752), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.8091, 0.2611, -0.4752),
                                       glm::dvec3(0.9352, 0.2611, 0.0),
                                       glm::dvec3(1.0, 0.45, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.9352, 0.2611, 0.0), glm::dvec3(0.8091, 0.2611, -0.4752),
      glm::dvec3(0.705, 0.1389, -0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.705, 0.1389, -0.414), glm::dvec3(0.8148, 0.1389, 0.0),
      glm::dvec3(0.9352, 0.2611, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8148, 0.1389, 0.0), glm::dvec3(0.705, 0.1389, -0.414),
      glm::dvec3(0.6489, 0.075, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 0.075, -0.3811), glm::dvec3(0.75, 0.075, 0.0),
      glm::dvec3(0.8148, 0.1389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8652, 0.45, -0.5081), glm::dvec3(0.5081, 0.45, -0.8652),
      glm::dvec3(0.4752, 0.2611, -0.8091), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4752, 0.2611, -0.8091), glm::dvec3(0.8091, 0.2611, -0.4752),
      glm::dvec3(0.8652, 0.45, -0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8091, 0.2611, -0.4752), glm::dvec3(0.4752, 0.2611, -0.8091),
      glm::dvec3(0.414, 0.1389, -0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.414, 0.1389, -0.705), glm::dvec3(0.705, 0.1389, -0.414),
      glm::dvec3(0.8091, 0.2611, -0.4752), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.705, 0.1389, -0.414), glm::dvec3(0.414, 0.1389, -0.705),
      glm::dvec3(0.3811, 0.075, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 0.075, -0.6489), glm::dvec3(0.6489, 0.075, -0.3811),
      glm::dvec3(0.705, 0.1389, -0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5081, 0.45, -0.8652), glm::dvec3(0.0, 0.45, -1.0),
      glm::dvec3(0.0, 0.2611, -0.9352), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.2611, -0.9352), glm::dvec3(0.4752, 0.2611, -0.8091),
      glm::dvec3(0.5081, 0.45, -0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4752, 0.2611, -0.8091), glm::dvec3(0.0, 0.2611, -0.9352),
      glm::dvec3(0.0, 0.1389, -0.8148), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.1389, -0.8148), glm::dvec3(0.414, 0.1389, -0.705),
      glm::dvec3(0.4752, 0.2611, -0.8091), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.414, 0.1389, -0.705),
                                       glm::dvec3(0.0, 0.1389, -0.8148),
                                       glm::dvec3(0.0, 0.075, -0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.075, -0.75), glm::dvec3(0.3811, 0.075, -0.6489),
      glm::dvec3(0.414, 0.1389, -0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.45, -1.0), glm::dvec3(-0.5081, 0.45, -0.8652),
      glm::dvec3(-0.4752, 0.2611, -0.8091), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4752, 0.2611, -0.8091),
                                       glm::dvec3(0.0, 0.2611, -0.9352),
                                       glm::dvec3(0.0, 0.45, -1.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.2611, -0.9352), glm::dvec3(-0.4752, 0.2611, -0.8091),
      glm::dvec3(-0.414, 0.1389, -0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.414, 0.1389, -0.705), glm::dvec3(0.0, 0.1389, -0.8148),
      glm::dvec3(0.0, 0.2611, -0.9352), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.1389, -0.8148), glm::dvec3(-0.414, 0.1389, -0.705),
      glm::dvec3(-0.3811, 0.075, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 0.075, -0.6489), glm::dvec3(0.0, 0.075, -0.75),
      glm::dvec3(0.0, 0.1389, -0.8148), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5081, 0.45, -0.8652), glm::dvec3(-0.8652, 0.45, -0.5081),
      glm::dvec3(-0.8091, 0.2611, -0.4752), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.8091, 0.2611, -0.4752),
                                       glm::dvec3(-0.4752, 0.2611, -0.8091),
                                       glm::dvec3(-0.5081, 0.45, -0.8652),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4752, 0.2611, -0.8091),
                                       glm::dvec3(-0.8091, 0.2611, -0.4752),
                                       glm::dvec3(-0.705, 0.1389, -0.414),
                                       tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.705, 0.1389, -0.414), glm::dvec3(-0.414, 0.1389, -0.705),
      glm::dvec3(-0.4752, 0.2611, -0.8091), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.414, 0.1389, -0.705), glm::dvec3(-0.705, 0.1389, -0.414),
      glm::dvec3(-0.6489, 0.075, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 0.075, -0.3811), glm::dvec3(-0.3811, 0.075, -0.6489),
      glm::dvec3(-0.414, 0.1389, -0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8652, 0.45, -0.5081), glm::dvec3(-1.0, 0.45, 0.0),
      glm::dvec3(-0.9352, 0.2611, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.9352, 0.2611, 0.0), glm::dvec3(-0.8091, 0.2611, -0.4752),
      glm::dvec3(-0.8652, 0.45, -0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8091, 0.2611, -0.4752), glm::dvec3(-0.9352, 0.2611, 0.0),
      glm::dvec3(-0.8148, 0.1389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8148, 0.1389, 0.0), glm::dvec3(-0.705, 0.1389, -0.414),
      glm::dvec3(-0.8091, 0.2611, -0.4752), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.705, 0.1389, -0.414),
                                       glm::dvec3(-0.8148, 0.1389, 0.0),
                                       glm::dvec3(-0.75, 0.075, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.75, 0.075, 0.0), glm::dvec3(-0.6489, 0.075, -0.3811),
      glm::dvec3(-0.705, 0.1389, -0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.0, 0.45, 0.0), glm::dvec3(-0.8652, 0.45, 0.5081),
      glm::dvec3(-0.8091, 0.2611, 0.4752), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.8091, 0.2611, 0.4752),
                                       glm::dvec3(-0.9352, 0.2611, 0.0),
                                       glm::dvec3(-1.0, 0.45, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.9352, 0.2611, 0.0), glm::dvec3(-0.8091, 0.2611, 0.4752),
      glm::dvec3(-0.705, 0.1389, 0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.705, 0.1389, 0.414), glm::dvec3(-0.8148, 0.1389, 0.0),
      glm::dvec3(-0.9352, 0.2611, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8148, 0.1389, 0.0), glm::dvec3(-0.705, 0.1389, 0.414),
      glm::dvec3(-0.6489, 0.075, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 0.075, 0.3811), glm::dvec3(-0.75, 0.075, 0.0),
      glm::dvec3(-0.8148, 0.1389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8652, 0.45, 0.5081), glm::dvec3(-0.5081, 0.45, 0.8652),
      glm::dvec3(-0.4752, 0.2611, 0.8091), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4752, 0.2611, 0.8091), glm::dvec3(-0.8091, 0.2611, 0.4752),
      glm::dvec3(-0.8652, 0.45, 0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8091, 0.2611, 0.4752), glm::dvec3(-0.4752, 0.2611, 0.8091),
      glm::dvec3(-0.414, 0.1389, 0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.414, 0.1389, 0.705), glm::dvec3(-0.705, 0.1389, 0.414),
      glm::dvec3(-0.8091, 0.2611, 0.4752), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.705, 0.1389, 0.414), glm::dvec3(-0.414, 0.1389, 0.705),
      glm::dvec3(-0.3811, 0.075, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 0.075, 0.6489), glm::dvec3(-0.6489, 0.075, 0.3811),
      glm::dvec3(-0.705, 0.1389, 0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5081, 0.45, 0.8652), glm::dvec3(0.0, 0.45, 1.0),
      glm::dvec3(0.0, 0.2611, 0.9352), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.2611, 0.9352), glm::dvec3(-0.4752, 0.2611, 0.8091),
      glm::dvec3(-0.5081, 0.45, 0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4752, 0.2611, 0.8091), glm::dvec3(0.0, 0.2611, 0.9352),
      glm::dvec3(0.0, 0.1389, 0.8148), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.1389, 0.8148), glm::dvec3(-0.414, 0.1389, 0.705),
      glm::dvec3(-0.4752, 0.2611, 0.8091), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.414, 0.1389, 0.705),
                                       glm::dvec3(0.0, 0.1389, 0.8148),
                                       glm::dvec3(0.0, 0.075, 0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.075, 0.75), glm::dvec3(-0.3811, 0.075, 0.6489),
      glm::dvec3(-0.414, 0.1389, 0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.45, 1.0), glm::dvec3(0.5081, 0.45, 0.8652),
      glm::dvec3(0.4752, 0.2611, 0.8091), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.4752, 0.2611, 0.8091),
                                       glm::dvec3(0.0, 0.2611, 0.9352),
                                       glm::dvec3(0.0, 0.45, 1.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.2611, 0.9352), glm::dvec3(0.4752, 0.2611, 0.8091),
      glm::dvec3(0.414, 0.1389, 0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.414, 0.1389, 0.705), glm::dvec3(0.0, 0.1389, 0.8148),
      glm::dvec3(0.0, 0.2611, 0.9352), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.1389, 0.8148), glm::dvec3(0.414, 0.1389, 0.705),
      glm::dvec3(0.3811, 0.075, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 0.075, 0.6489), glm::dvec3(0.0, 0.075, 0.75),
      glm::dvec3(0.0, 0.1389, 0.8148), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5081, 0.45, 0.8652), glm::dvec3(0.8652, 0.45, 0.5081),
      glm::dvec3(0.8091, 0.2611, 0.4752), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8091, 0.2611, 0.4752), glm::dvec3(0.4752, 0.2611, 0.8091),
      glm::dvec3(0.5081, 0.45, 0.8652), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4752, 0.2611, 0.8091), glm::dvec3(0.8091, 0.2611, 0.4752),
      glm::dvec3(0.705, 0.1389, 0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.705, 0.1389, 0.414), glm::dvec3(0.414, 0.1389, 0.705),
      glm::dvec3(0.4752, 0.2611, 0.8091), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.414, 0.1389, 0.705), glm::dvec3(0.705, 0.1389, 0.414),
      glm::dvec3(0.6489, 0.075, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 0.075, 0.3811), glm::dvec3(0.3811, 0.075, 0.6489),
      glm::dvec3(0.414, 0.1389, 0.705), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8652, 0.45, 0.5081), glm::dvec3(1.0, 0.45, 0.0),
      glm::dvec3(0.9352, 0.2611, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.9352, 0.2611, 0.0), glm::dvec3(0.8091, 0.2611, 0.4752),
      glm::dvec3(0.8652, 0.45, 0.5081), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8091, 0.2611, 0.4752), glm::dvec3(0.9352, 0.2611, 0.0),
      glm::dvec3(0.8148, 0.1389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.8148, 0.1389, 0.0), glm::dvec3(0.705, 0.1389, 0.414),
      glm::dvec3(0.8091, 0.2611, 0.4752), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.705, 0.1389, 0.414),
                                       glm::dvec3(0.8148, 0.1389, 0.0),
                                       glm::dvec3(0.75, 0.075, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.75, 0.075, 0.0), glm::dvec3(0.6489, 0.075, 0.3811),
      glm::dvec3(0.705, 0.1389, 0.414), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.75, 0.075, 0.0), glm::dvec3(0.6489, 0.075, -0.3811),
      glm::dvec3(0.6176, 0.0389, -0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.6176, 0.0389, -0.3628),
                                       glm::dvec3(0.7139, 0.0389, 0.0),
                                       glm::dvec3(0.75, 0.075, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.7139, 0.0389, 0.0), glm::dvec3(0.6176, 0.0389, -0.3628),
      glm::dvec3(0.4422, 0.0111, -0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4422, 0.0111, -0.2597), glm::dvec3(0.5111, 0.0111, 0.0),
      glm::dvec3(0.7139, 0.0389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.5111, 0.0111, 0.0),
                                       glm::dvec3(0.4422, 0.0111, -0.2597),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.5111, 0.0111, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 0.075, -0.3811), glm::dvec3(0.3811, 0.075, -0.6489),
      glm::dvec3(0.3628, 0.0389, -0.6176), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3628, 0.0389, -0.6176), glm::dvec3(0.6176, 0.0389, -0.3628),
      glm::dvec3(0.6489, 0.075, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6176, 0.0389, -0.3628), glm::dvec3(0.3628, 0.0389, -0.6176),
      glm::dvec3(0.2597, 0.0111, -0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2597, 0.0111, -0.4422), glm::dvec3(0.4422, 0.0111, -0.2597),
      glm::dvec3(0.6176, 0.0389, -0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.4422, 0.0111, -0.2597),
                                       glm::dvec3(0.2597, 0.0111, -0.4422),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.4422, 0.0111, -0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 0.075, -0.6489), glm::dvec3(0.0, 0.075, -0.75),
      glm::dvec3(0.0, 0.0389, -0.7139), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.0389, -0.7139), glm::dvec3(0.3628, 0.0389, -0.6176),
      glm::dvec3(0.3811, 0.075, -0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3628, 0.0389, -0.6176), glm::dvec3(0.0, 0.0389, -0.7139),
      glm::dvec3(0.0, 0.0111, -0.5111), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.0111, -0.5111), glm::dvec3(0.2597, 0.0111, -0.4422),
      glm::dvec3(0.3628, 0.0389, -0.6176), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.2597, 0.0111, -0.4422),
                                       glm::dvec3(0.0, 0.0111, -0.5111),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.2597, 0.0111, -0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.075, -0.75), glm::dvec3(-0.3811, 0.075, -0.6489),
      glm::dvec3(-0.3628, 0.0389, -0.6176), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.3628, 0.0389, -0.6176),
                                       glm::dvec3(0.0, 0.0389, -0.7139),
                                       glm::dvec3(0.0, 0.075, -0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.0389, -0.7139), glm::dvec3(-0.3628, 0.0389, -0.6176),
      glm::dvec3(-0.2597, 0.0111, -0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.2597, 0.0111, -0.4422), glm::dvec3(0.0, 0.0111, -0.5111),
      glm::dvec3(0.0, 0.0389, -0.7139), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0, 0.0111, -0.5111),
                                       glm::dvec3(-0.2597, 0.0111, -0.4422),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.0, 0.0111, -0.5111), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 0.075, -0.6489), glm::dvec3(-0.6489, 0.075, -0.3811),
      glm::dvec3(-0.6176, 0.0389, -0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.6176, 0.0389, -0.3628),
                                       glm::dvec3(-0.3628, 0.0389, -0.6176),
                                       glm::dvec3(-0.3811, 0.075, -0.6489),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.3628, 0.0389, -0.6176),
                                       glm::dvec3(-0.6176, 0.0389, -0.3628),
                                       glm::dvec3(-0.4422, 0.0111, -0.2597),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4422, 0.0111, -0.2597),
                                       glm::dvec3(-0.2597, 0.0111, -0.4422),
                                       glm::dvec3(-0.3628, 0.0389, -0.6176),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.2597, 0.0111, -0.4422),
                                       glm::dvec3(-0.4422, 0.0111, -0.2597),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(-0.2597, 0.0111, -0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 0.075, -0.3811), glm::dvec3(-0.75, 0.075, 0.0),
      glm::dvec3(-0.7139, 0.0389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.7139, 0.0389, 0.0), glm::dvec3(-0.6176, 0.0389, -0.3628),
      glm::dvec3(-0.6489, 0.075, -0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6176, 0.0389, -0.3628), glm::dvec3(-0.7139, 0.0389, 0.0),
      glm::dvec3(-0.5111, 0.0111, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5111, 0.0111, 0.0), glm::dvec3(-0.4422, 0.0111, -0.2597),
      glm::dvec3(-0.6176, 0.0389, -0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4422, 0.0111, -0.2597),
                                       glm::dvec3(-0.5111, 0.0111, 0.0),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(-0.4422, 0.0111, -0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.75, 0.075, 0.0), glm::dvec3(-0.6489, 0.075, 0.3811),
      glm::dvec3(-0.6176, 0.0389, 0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.6176, 0.0389, 0.3628),
                                       glm::dvec3(-0.7139, 0.0389, 0.0),
                                       glm::dvec3(-0.75, 0.075, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.7139, 0.0389, 0.0), glm::dvec3(-0.6176, 0.0389, 0.3628),
      glm::dvec3(-0.4422, 0.0111, 0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4422, 0.0111, 0.2597), glm::dvec3(-0.5111, 0.0111, 0.0),
      glm::dvec3(-0.7139, 0.0389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.5111, 0.0111, 0.0),
                                       glm::dvec3(-0.4422, 0.0111, 0.2597),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(-0.5111, 0.0111, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6489, 0.075, 0.3811), glm::dvec3(-0.3811, 0.075, 0.6489),
      glm::dvec3(-0.3628, 0.0389, 0.6176), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3628, 0.0389, 0.6176), glm::dvec3(-0.6176, 0.0389, 0.3628),
      glm::dvec3(-0.6489, 0.075, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.6176, 0.0389, 0.3628), glm::dvec3(-0.3628, 0.0389, 0.6176),
      glm::dvec3(-0.2597, 0.0111, 0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.2597, 0.0111, 0.4422), glm::dvec3(-0.4422, 0.0111, 0.2597),
      glm::dvec3(-0.6176, 0.0389, 0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4422, 0.0111, 0.2597),
                                       glm::dvec3(-0.2597, 0.0111, 0.4422),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(-0.4422, 0.0111, 0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3811, 0.075, 0.6489), glm::dvec3(0.0, 0.075, 0.75),
      glm::dvec3(0.0, 0.0389, 0.7139), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.0389, 0.7139), glm::dvec3(-0.3628, 0.0389, 0.6176),
      glm::dvec3(-0.3811, 0.075, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3628, 0.0389, 0.6176), glm::dvec3(0.0, 0.0389, 0.7139),
      glm::dvec3(0.0, 0.0111, 0.5111), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.0111, 0.5111), glm::dvec3(-0.2597, 0.0111, 0.4422),
      glm::dvec3(-0.3628, 0.0389, 0.6176), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.2597, 0.0111, 0.4422),
                                       glm::dvec3(0.0, 0.0111, 0.5111),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(-0.2597, 0.0111, 0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.075, 0.75), glm::dvec3(0.3811, 0.075, 0.6489),
      glm::dvec3(0.3628, 0.0389, 0.6176), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.3628, 0.0389, 0.6176),
                                       glm::dvec3(0.0, 0.0389, 0.7139),
                                       glm::dvec3(0.0, 0.075, 0.75), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 0.0389, 0.7139), glm::dvec3(0.3628, 0.0389, 0.6176),
      glm::dvec3(0.2597, 0.0111, 0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2597, 0.0111, 0.4422), glm::dvec3(0.0, 0.0111, 0.5111),
      glm::dvec3(0.0, 0.0389, 0.7139), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0, 0.0111, 0.5111),
                                       glm::dvec3(0.2597, 0.0111, 0.4422),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.0, 0.0111, 0.5111), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3811, 0.075, 0.6489), glm::dvec3(0.6489, 0.075, 0.3811),
      glm::dvec3(0.6176, 0.0389, 0.3628), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6176, 0.0389, 0.3628), glm::dvec3(0.3628, 0.0389, 0.6176),
      glm::dvec3(0.3811, 0.075, 0.6489), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3628, 0.0389, 0.6176), glm::dvec3(0.6176, 0.0389, 0.3628),
      glm::dvec3(0.4422, 0.0111, 0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4422, 0.0111, 0.2597), glm::dvec3(0.2597, 0.0111, 0.4422),
      glm::dvec3(0.3628, 0.0389, 0.6176), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.2597, 0.0111, 0.4422),
                                       glm::dvec3(0.4422, 0.0111, 0.2597),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.2597, 0.0111, 0.4422), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6489, 0.075, 0.3811), glm::dvec3(0.75, 0.075, 0.0),
      glm::dvec3(0.7139, 0.0389, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.7139, 0.0389, 0.0), glm::dvec3(0.6176, 0.0389, 0.3628),
      glm::dvec3(0.6489, 0.075, 0.3811), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.6176, 0.0389, 0.3628), glm::dvec3(0.7139, 0.0389, 0.0),
      glm::dvec3(0.5111, 0.0111, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5111, 0.0111, 0.0), glm::dvec3(0.4422, 0.0111, 0.2597),
      glm::dvec3(0.6176, 0.0389, 0.3628), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.4422, 0.0111, 0.2597),
                                       glm::dvec3(0.5111, 0.0111, 0.0),
                                       glm::dvec3(0.0, -0.0, -0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, -0.0, -0.0), glm::dvec3(0.0, -0.0, -0.0),
      glm::dvec3(0.4422, 0.0111, 0.2597), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.8, 1.0125, 0.0), glm::dvec3(-0.787, 1.0417, -0.1),
      glm::dvec3(-1.1159, 1.0364, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.1159, 1.0364, -0.1),
                                       glm::dvec3(-1.0981, 1.0083, 0.0),
                                       glm::dvec3(-0.8, 1.0125, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.0981, 1.0083, 0.0), glm::dvec3(-1.1159, 1.0364, -0.1),
      glm::dvec3(-1.3193, 0.9997, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3193, 0.9997, -0.1), glm::dvec3(-1.2852, 0.9792, 0.0),
      glm::dvec3(-1.0981, 1.0083, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2852, 0.9792, 0.0), glm::dvec3(-1.3193, 0.9997, -0.1),
      glm::dvec3(-1.3889, 0.9, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3889, 0.9, -0.1), glm::dvec3(-1.35, 0.9, 0.0),
      glm::dvec3(-1.2852, 0.9792, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.787, 1.0417, -0.1), glm::dvec3(-0.763, 1.0958, -0.1),
      glm::dvec3(-1.1489, 1.0886, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1489, 1.0886, -0.1), glm::dvec3(-1.1159, 1.0364, -0.1),
      glm::dvec3(-0.787, 1.0417, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1159, 1.0364, -0.1), glm::dvec3(-1.1489, 1.0886, -0.1),
      glm::dvec3(-1.3826, 1.0378, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3826, 1.0378, -0.1), glm::dvec3(-1.3193, 0.9997, -0.1),
      glm::dvec3(-1.1159, 1.0364, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3193, 0.9997, -0.1), glm::dvec3(-1.3826, 1.0378, -0.1),
      glm::dvec3(-1.4611, 0.9, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4611, 0.9, -0.1), glm::dvec3(-1.3889, 0.9, -0.1),
      glm::dvec3(-1.3193, 0.9997, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.763, 1.0958, -0.1), glm::dvec3(-0.75, 1.125, 0.0),
      glm::dvec3(-1.1667, 1.1167, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1667, 1.1167, 0.0), glm::dvec3(-1.1489, 1.0886, -0.1),
      glm::dvec3(-0.763, 1.0958, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1489, 1.0886, -0.1), glm::dvec3(-1.1667, 1.1167, 0.0),
      glm::dvec3(-1.4167, 1.0583, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4167, 1.0583, 0.0), glm::dvec3(-1.3826, 1.0378, -0.1),
      glm::dvec3(-1.1489, 1.0886, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.3826, 1.0378, -0.1),
                                       glm::dvec3(-1.4167, 1.0583, 0.0),
                                       glm::dvec3(-1.5, 0.9, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.5, 0.9, 0.0), glm::dvec3(-1.4611, 0.9, -0.1),
      glm::dvec3(-1.3826, 1.0378, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.75, 1.125, 0.0), glm::dvec3(-0.763, 1.0958, 0.1),
      glm::dvec3(-1.1489, 1.0886, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.1489, 1.0886, 0.1),
                                       glm::dvec3(-1.1667, 1.1167, 0.0),
                                       glm::dvec3(-0.75, 1.125, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1667, 1.1167, 0.0), glm::dvec3(-1.1489, 1.0886, 0.1),
      glm::dvec3(-1.3826, 1.0378, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3826, 1.0378, 0.1), glm::dvec3(-1.4167, 1.0583, 0.0),
      glm::dvec3(-1.1667, 1.1167, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.4167, 1.0583, 0.0),
                                       glm::dvec3(-1.3826, 1.0378, 0.1),
                                       glm::dvec3(-1.4611, 0.9, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4611, 0.9, 0.1), glm::dvec3(-1.5, 0.9, 0.0),
      glm::dvec3(-1.4167, 1.0583, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.763, 1.0958, 0.1), glm::dvec3(-0.787, 1.0417, 0.1),
      glm::dvec3(-1.1159, 1.0364, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1159, 1.0364, 0.1), glm::dvec3(-1.1489, 1.0886, 0.1),
      glm::dvec3(-0.763, 1.0958, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1489, 1.0886, 0.1), glm::dvec3(-1.1159, 1.0364, 0.1),
      glm::dvec3(-1.3193, 0.9997, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3193, 0.9997, 0.1), glm::dvec3(-1.3826, 1.0378, 0.1),
      glm::dvec3(-1.1489, 1.0886, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.3826, 1.0378, 0.1),
                                       glm::dvec3(-1.3193, 0.9997, 0.1),
                                       glm::dvec3(-1.3889, 0.9, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3889, 0.9, 0.1), glm::dvec3(-1.4611, 0.9, 0.1),
      glm::dvec3(-1.3826, 1.0378, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.787, 1.0417, 0.1), glm::dvec3(-0.8, 1.0125, 0.0),
      glm::dvec3(-1.0981, 1.0083, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.0981, 1.0083, 0.0), glm::dvec3(-1.1159, 1.0364, 0.1),
      glm::dvec3(-0.787, 1.0417, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.1159, 1.0364, 0.1), glm::dvec3(-1.0981, 1.0083, 0.0),
      glm::dvec3(-1.2852, 0.9792, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2852, 0.9792, 0.0), glm::dvec3(-1.3193, 0.9997, 0.1),
      glm::dvec3(-1.1159, 1.0364, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.3193, 0.9997, 0.1),
                                       glm::dvec3(-1.2852, 0.9792, 0.0),
                                       glm::dvec3(-1.35, 0.9, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.35, 0.9, 0.0), glm::dvec3(-1.3889, 0.9, 0.1),
      glm::dvec3(-1.3193, 0.9997, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.35, 0.9, 0.0), glm::dvec3(-1.3889, 0.9, -0.1),
      glm::dvec3(-1.3475, 0.7385, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.3475, 0.7385, -0.1),
                                       glm::dvec3(-1.3148, 0.7583, 0.0),
                                       glm::dvec3(-1.35, 0.9, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3148, 0.7583, 0.0), glm::dvec3(-1.3475, 0.7385, -0.1),
      glm::dvec3(-1.2167, 0.5629, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2167, 0.5629, -0.1), glm::dvec3(-1.2019, 0.5917, 0.0),
      glm::dvec3(-1.3148, 0.7583, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2019, 0.5917, 0.0), glm::dvec3(-1.2167, 0.5629, -0.1),
      glm::dvec3(-0.987, 0.4111, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.987, 0.4111, -0.1), glm::dvec3(-1.0, 0.45, 0.0),
      glm::dvec3(-1.2019, 0.5917, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3889, 0.9, -0.1), glm::dvec3(-1.4611, 0.9, -0.1),
      glm::dvec3(-1.4081, 0.7017, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4081, 0.7017, -0.1), glm::dvec3(-1.3475, 0.7385, -0.1),
      glm::dvec3(-1.3889, 0.9, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3475, 0.7385, -0.1), glm::dvec3(-1.4081, 0.7017, -0.1),
      glm::dvec3(-1.2444, 0.5094, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2444, 0.5094, -0.1), glm::dvec3(-1.2167, 0.5629, -0.1),
      glm::dvec3(-1.3475, 0.7385, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2167, 0.5629, -0.1), glm::dvec3(-1.2444, 0.5094, -0.1),
      glm::dvec3(-0.963, 0.3389, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.963, 0.3389, -0.1), glm::dvec3(-0.987, 0.4111, -0.1),
      glm::dvec3(-1.2167, 0.5629, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4611, 0.9, -0.1), glm::dvec3(-1.5, 0.9, 0.0),
      glm::dvec3(-1.4407, 0.6819, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4407, 0.6819, 0.0), glm::dvec3(-1.4081, 0.7017, -0.1),
      glm::dvec3(-1.4611, 0.9, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4081, 0.7017, -0.1), glm::dvec3(-1.4407, 0.6819, 0.0),
      glm::dvec3(-1.2593, 0.4806, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2593, 0.4806, 0.0), glm::dvec3(-1.2444, 0.5094, -0.1),
      glm::dvec3(-1.4081, 0.7017, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.2444, 0.5094, -0.1),
                                       glm::dvec3(-1.2593, 0.4806, 0.0),
                                       glm::dvec3(-0.95, 0.3, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.95, 0.3, 0.0), glm::dvec3(-0.963, 0.3389, -0.1),
      glm::dvec3(-1.2444, 0.5094, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.5, 0.9, 0.0), glm::dvec3(-1.4611, 0.9, 0.1),
      glm::dvec3(-1.4081, 0.7017, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.4081, 0.7017, 0.1),
                                       glm::dvec3(-1.4407, 0.6819, 0.0),
                                       glm::dvec3(-1.5, 0.9, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4407, 0.6819, 0.0), glm::dvec3(-1.4081, 0.7017, 0.1),
      glm::dvec3(-1.2444, 0.5094, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2444, 0.5094, 0.1), glm::dvec3(-1.2593, 0.4806, 0.0),
      glm::dvec3(-1.4407, 0.6819, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2593, 0.4806, 0.0), glm::dvec3(-1.2444, 0.5094, 0.1),
      glm::dvec3(-0.963, 0.3389, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.963, 0.3389, 0.1), glm::dvec3(-0.95, 0.3, 0.0),
      glm::dvec3(-1.2593, 0.4806, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4611, 0.9, 0.1), glm::dvec3(-1.3889, 0.9, 0.1),
      glm::dvec3(-1.3475, 0.7385, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.3475, 0.7385, 0.1),
                                       glm::dvec3(-1.4081, 0.7017, 0.1),
                                       glm::dvec3(-1.4611, 0.9, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.4081, 0.7017, 0.1), glm::dvec3(-1.3475, 0.7385, 0.1),
      glm::dvec3(-1.2167, 0.5629, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2167, 0.5629, 0.1), glm::dvec3(-1.2444, 0.5094, 0.1),
      glm::dvec3(-1.4081, 0.7017, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2444, 0.5094, 0.1), glm::dvec3(-1.2167, 0.5629, 0.1),
      glm::dvec3(-0.987, 0.4111, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.987, 0.4111, 0.1), glm::dvec3(-0.963, 0.3389, 0.1),
      glm::dvec3(-1.2444, 0.5094, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3889, 0.9, 0.1), glm::dvec3(-1.35, 0.9, 0.0),
      glm::dvec3(-1.3148, 0.7583, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.3148, 0.7583, 0.0),
                                       glm::dvec3(-1.3475, 0.7385, 0.1),
                                       glm::dvec3(-1.3889, 0.9, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.3475, 0.7385, 0.1), glm::dvec3(-1.3148, 0.7583, 0.0),
      glm::dvec3(-1.2019, 0.5917, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.2019, 0.5917, 0.0), glm::dvec3(-1.2167, 0.5629, 0.1),
      glm::dvec3(-1.3475, 0.7385, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-1.2167, 0.5629, 0.1),
                                       glm::dvec3(-1.2019, 0.5917, 0.0),
                                       glm::dvec3(-1.0, 0.45, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-1.0, 0.45, 0.0), glm::dvec3(-0.987, 0.4111, 0.1),
      glm::dvec3(-1.2167, 0.5629, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.85, 0.7125, 0.0), glm::dvec3(0.85, 0.6056, -0.22),
      glm::dvec3(1.1698, 0.7371, -0.1846), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.1698, 0.7371, -0.1846),
                                       glm::dvec3(1.1352, 0.8056, 0.0),
                                       glm::dvec3(0.85, 0.7125, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.1352, 0.8056, 0.0), glm::dvec3(1.1698, 0.7371, -0.1846),
      glm::dvec3(1.2747, 0.9814, -0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2747, 0.9814, -0.1188), glm::dvec3(1.2315, 1.0069, 0.0),
      glm::dvec3(1.1352, 0.8056, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2315, 1.0069, 0.0), glm::dvec3(1.2747, 0.9814, -0.1188),
      glm::dvec3(1.4278, 1.2, -0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4278, 1.2, -0.0833), glm::dvec3(1.35, 1.2, 0.0),
      glm::dvec3(1.2315, 1.0069, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.85, 0.6056, -0.22), glm::dvec3(0.85, 0.4069, -0.22),
      glm::dvec3(1.234, 0.6101, -0.1846), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.234, 0.6101, -0.1846), glm::dvec3(1.1698, 0.7371, -0.1846),
      glm::dvec3(0.85, 0.6056, -0.22), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.1698, 0.7371, -0.1846), glm::dvec3(1.234, 0.6101, -0.1846),
      glm::dvec3(1.3549, 0.9339, -0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3549, 0.9339, -0.1188), glm::dvec3(1.2747, 0.9814, -0.1188),
      glm::dvec3(1.1698, 0.7371, -0.1846), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2747, 0.9814, -0.1188), glm::dvec3(1.3549, 0.9339, -0.1188),
      glm::dvec3(1.5722, 1.2, -0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.5722, 1.2, -0.0833), glm::dvec3(1.4278, 1.2, -0.0833),
      glm::dvec3(1.2747, 0.9814, -0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.85, 0.4069, -0.22), glm::dvec3(0.85, 0.3, 0.0),
      glm::dvec3(1.2685, 0.5417, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2685, 0.5417, 0.0), glm::dvec3(1.234, 0.6101, -0.1846),
      glm::dvec3(0.85, 0.4069, -0.22), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.234, 0.6101, -0.1846), glm::dvec3(1.2685, 0.5417, 0.0),
      glm::dvec3(1.3981, 0.9083, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3981, 0.9083, 0.0), glm::dvec3(1.3549, 0.9339, -0.1188),
      glm::dvec3(1.234, 0.6101, -0.1846), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.3549, 0.9339, -0.1188),
                                       glm::dvec3(1.3981, 0.9083, 0.0),
                                       glm::dvec3(1.65, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.65, 1.2, 0.0), glm::dvec3(1.5722, 1.2, -0.0833),
      glm::dvec3(1.3549, 0.9339, -0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.85, 0.3, 0.0), glm::dvec3(0.85, 0.4069, 0.22),
      glm::dvec3(1.234, 0.6101, 0.1846), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.234, 0.6101, 0.1846),
                                       glm::dvec3(1.2685, 0.5417, 0.0),
                                       glm::dvec3(0.85, 0.3, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2685, 0.5417, 0.0), glm::dvec3(1.234, 0.6101, 0.1846),
      glm::dvec3(1.3549, 0.9339, 0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3549, 0.9339, 0.1188), glm::dvec3(1.3981, 0.9083, 0.0),
      glm::dvec3(1.2685, 0.5417, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3981, 0.9083, 0.0), glm::dvec3(1.3549, 0.9339, 0.1188),
      glm::dvec3(1.5722, 1.2, 0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.5722, 1.2, 0.0833), glm::dvec3(1.65, 1.2, 0.0),
      glm::dvec3(1.3981, 0.9083, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.85, 0.4069, 0.22), glm::dvec3(0.85, 0.6056, 0.22),
      glm::dvec3(1.1698, 0.7371, 0.1846), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.1698, 0.7371, 0.1846), glm::dvec3(1.234, 0.6101, 0.1846),
      glm::dvec3(0.85, 0.4069, 0.22), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.234, 0.6101, 0.1846), glm::dvec3(1.1698, 0.7371, 0.1846),
      glm::dvec3(1.2747, 0.9814, 0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2747, 0.9814, 0.1188), glm::dvec3(1.3549, 0.9339, 0.1188),
      glm::dvec3(1.234, 0.6101, 0.1846), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3549, 0.9339, 0.1188), glm::dvec3(1.2747, 0.9814, 0.1188),
      glm::dvec3(1.4278, 1.2, 0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4278, 1.2, 0.0833), glm::dvec3(1.5722, 1.2, 0.0833),
      glm::dvec3(1.3549, 0.9339, 0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.85, 0.6056, 0.22), glm::dvec3(0.85, 0.7125, 0.0),
      glm::dvec3(1.1352, 0.8056, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.1352, 0.8056, 0.0), glm::dvec3(1.1698, 0.7371, 0.1846),
      glm::dvec3(0.85, 0.6056, 0.22), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.1698, 0.7371, 0.1846), glm::dvec3(1.1352, 0.8056, 0.0),
      glm::dvec3(1.2315, 1.0069, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.2315, 1.0069, 0.0), glm::dvec3(1.2747, 0.9814, 0.1188),
      glm::dvec3(1.1698, 0.7371, 0.1846), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.2747, 0.9814, 0.1188),
                                       glm::dvec3(1.2315, 1.0069, 0.0),
                                       glm::dvec3(1.35, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.35, 1.2, 0.0), glm::dvec3(1.4278, 1.2, 0.0833),
      glm::dvec3(1.2747, 0.9814, 0.1188), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.35, 1.2, 0.0), glm::dvec3(1.4278, 1.2, -0.0833),
      glm::dvec3(1.4789, 1.2272, -0.0747), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.4789, 1.2272, -0.0747),
                                       glm::dvec3(1.3963, 1.225, 0.0),
                                       glm::dvec3(1.35, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3963, 1.225, 0.0), glm::dvec3(1.4789, 1.2272, -0.0747),
      glm::dvec3(1.4912, 1.2277, -0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4912, 1.2277, -0.0586), glm::dvec3(1.4204, 1.225, 0.0),
      glm::dvec3(1.3963, 1.225, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4204, 1.225, 0.0), glm::dvec3(1.4912, 1.2277, -0.0586),
      glm::dvec3(1.4519, 1.2, -0.05), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4519, 1.2, -0.05), glm::dvec3(1.4, 1.2, 0.0),
      glm::dvec3(1.4204, 1.225, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4278, 1.2, -0.0833), glm::dvec3(1.5722, 1.2, -0.0833),
      glm::dvec3(1.6322, 1.2312, -0.0747), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6322, 1.2312, -0.0747), glm::dvec3(1.4789, 1.2272, -0.0747),
      glm::dvec3(1.4278, 1.2, -0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4789, 1.2272, -0.0747), glm::dvec3(1.6322, 1.2312, -0.0747),
      glm::dvec3(1.6227, 1.2327, -0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6227, 1.2327, -0.0586), glm::dvec3(1.4912, 1.2277, -0.0586),
      glm::dvec3(1.4789, 1.2272, -0.0747), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4912, 1.2277, -0.0586), glm::dvec3(1.6227, 1.2327, -0.0586),
      glm::dvec3(1.5481, 1.2, -0.05), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.5481, 1.2, -0.05), glm::dvec3(1.4519, 1.2, -0.05),
      glm::dvec3(1.4912, 1.2277, -0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.5722, 1.2, -0.0833), glm::dvec3(1.65, 1.2, 0.0),
      glm::dvec3(1.7148, 1.2333, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.7148, 1.2333, 0.0), glm::dvec3(1.6322, 1.2312, -0.0747),
      glm::dvec3(1.5722, 1.2, -0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6322, 1.2312, -0.0747), glm::dvec3(1.7148, 1.2333, 0.0),
      glm::dvec3(1.6935, 1.2354, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6935, 1.2354, 0.0), glm::dvec3(1.6227, 1.2327, -0.0586),
      glm::dvec3(1.6322, 1.2312, -0.0747), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.6227, 1.2327, -0.0586),
                                       glm::dvec3(1.6935, 1.2354, 0.0),
                                       glm::dvec3(1.6, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6, 1.2, 0.0), glm::dvec3(1.5481, 1.2, -0.05),
      glm::dvec3(1.6227, 1.2327, -0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.65, 1.2, 0.0), glm::dvec3(1.5722, 1.2, 0.0833),
      glm::dvec3(1.6322, 1.2312, 0.0747), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.6322, 1.2312, 0.0747),
                                       glm::dvec3(1.7148, 1.2333, 0.0),
                                       glm::dvec3(1.65, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.7148, 1.2333, 0.0), glm::dvec3(1.6322, 1.2312, 0.0747),
      glm::dvec3(1.6227, 1.2327, 0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6227, 1.2327, 0.0586), glm::dvec3(1.6935, 1.2354, 0.0),
      glm::dvec3(1.7148, 1.2333, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.6935, 1.2354, 0.0),
                                       glm::dvec3(1.6227, 1.2327, 0.0586),
                                       glm::dvec3(1.5481, 1.2, 0.05), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.5481, 1.2, 0.05), glm::dvec3(1.6, 1.2, 0.0),
      glm::dvec3(1.6935, 1.2354, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.5722, 1.2, 0.0833), glm::dvec3(1.4278, 1.2, 0.0833),
      glm::dvec3(1.4789, 1.2272, 0.0747), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4789, 1.2272, 0.0747), glm::dvec3(1.6322, 1.2312, 0.0747),
      glm::dvec3(1.5722, 1.2, 0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.6322, 1.2312, 0.0747), glm::dvec3(1.4789, 1.2272, 0.0747),
      glm::dvec3(1.4912, 1.2277, 0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4912, 1.2277, 0.0586), glm::dvec3(1.6227, 1.2327, 0.0586),
      glm::dvec3(1.6322, 1.2312, 0.0747), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.6227, 1.2327, 0.0586),
                                       glm::dvec3(1.4912, 1.2277, 0.0586),
                                       glm::dvec3(1.4519, 1.2, 0.05), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4519, 1.2, 0.05), glm::dvec3(1.5481, 1.2, 0.05),
      glm::dvec3(1.6227, 1.2327, 0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4278, 1.2, 0.0833), glm::dvec3(1.35, 1.2, 0.0),
      glm::dvec3(1.3963, 1.225, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.3963, 1.225, 0.0), glm::dvec3(1.4789, 1.2272, 0.0747),
      glm::dvec3(1.4278, 1.2, 0.0833), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4789, 1.2272, 0.0747), glm::dvec3(1.3963, 1.225, 0.0),
      glm::dvec3(1.4204, 1.225, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4204, 1.225, 0.0), glm::dvec3(1.4912, 1.2277, 0.0586),
      glm::dvec3(1.4789, 1.2272, 0.0747), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(1.4912, 1.2277, 0.0586),
                                       glm::dvec3(1.4204, 1.225, 0.0),
                                       glm::dvec3(1.4, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(1.4, 1.2, 0.0), glm::dvec3(1.4519, 1.2, 0.05),
      glm::dvec3(1.4912, 1.2277, 0.0586), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.1571, 1.5333, -0.0924), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.1571, 1.5333, -0.0924),
                                       glm::dvec3(0.1815, 1.5333, 0.0),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1815, 1.5333, 0.0), glm::dvec3(0.1571, 1.5333, -0.0924),
      glm::dvec3(0.1026, 1.4417, -0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1026, 1.4417, -0.0603), glm::dvec3(0.1185, 1.4417, 0.0),
      glm::dvec3(0.1815, 1.5333, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1185, 1.4417, 0.0), glm::dvec3(0.1026, 1.4417, -0.0603),
      glm::dvec3(0.0865, 1.35, -0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0865, 1.35, -0.0508), glm::dvec3(0.1, 1.35, 0.0),
      glm::dvec3(0.1185, 1.4417, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.0924, 1.5333, -0.1571), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0924, 1.5333, -0.1571),
                                       glm::dvec3(0.1571, 1.5333, -0.0924),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1571, 1.5333, -0.0924), glm::dvec3(0.0924, 1.5333, -0.1571),
      glm::dvec3(0.0603, 1.4417, -0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0603, 1.4417, -0.1026), glm::dvec3(0.1026, 1.4417, -0.0603),
      glm::dvec3(0.1571, 1.5333, -0.0924), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1026, 1.4417, -0.0603), glm::dvec3(0.0603, 1.4417, -0.1026),
      glm::dvec3(0.0508, 1.35, -0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0508, 1.35, -0.0865), glm::dvec3(0.0865, 1.35, -0.0508),
      glm::dvec3(0.1026, 1.4417, -0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.0, 1.5333, -0.1815), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0, 1.5333, -0.1815),
                                       glm::dvec3(0.0924, 1.5333, -0.1571),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0924, 1.5333, -0.1571), glm::dvec3(0.0, 1.5333, -0.1815),
      glm::dvec3(0.0, 1.4417, -0.1185), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.4417, -0.1185), glm::dvec3(0.0603, 1.4417, -0.1026),
      glm::dvec3(0.0924, 1.5333, -0.1571), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0603, 1.4417, -0.1026),
                                       glm::dvec3(0.0, 1.4417, -0.1185),
                                       glm::dvec3(0.0, 1.35, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.35, -0.1), glm::dvec3(0.0508, 1.35, -0.0865),
      glm::dvec3(0.0603, 1.4417, -0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(-0.0924, 1.5333, -0.1571), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.0924, 1.5333, -0.1571),
                                       glm::dvec3(0.0, 1.5333, -0.1815),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.5333, -0.1815), glm::dvec3(-0.0924, 1.5333, -0.1571),
      glm::dvec3(-0.0603, 1.4417, -0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0603, 1.4417, -0.1026), glm::dvec3(0.0, 1.4417, -0.1185),
      glm::dvec3(0.0, 1.5333, -0.1815), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.4417, -0.1185), glm::dvec3(-0.0603, 1.4417, -0.1026),
      glm::dvec3(-0.0508, 1.35, -0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0508, 1.35, -0.0865), glm::dvec3(0.0, 1.35, -0.1),
      glm::dvec3(0.0, 1.4417, -0.1185), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(-0.1571, 1.5333, -0.0924), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1571, 1.5333, -0.0924),
                                       glm::dvec3(-0.0924, 1.5333, -0.1571),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.0924, 1.5333, -0.1571),
                                       glm::dvec3(-0.1571, 1.5333, -0.0924),
                                       glm::dvec3(-0.1026, 1.4417, -0.0603),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1026, 1.4417, -0.0603),
                                       glm::dvec3(-0.0603, 1.4417, -0.1026),
                                       glm::dvec3(-0.0924, 1.5333, -0.1571),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.0603, 1.4417, -0.1026),
                                       glm::dvec3(-0.1026, 1.4417, -0.0603),
                                       glm::dvec3(-0.0865, 1.35, -0.0508),
                                       tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0865, 1.35, -0.0508), glm::dvec3(-0.0508, 1.35, -0.0865),
      glm::dvec3(-0.0603, 1.4417, -0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(-0.1815, 1.5333, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1815, 1.5333, 0.0),
                                       glm::dvec3(-0.1571, 1.5333, -0.0924),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1571, 1.5333, -0.0924), glm::dvec3(-0.1815, 1.5333, 0.0),
      glm::dvec3(-0.1185, 1.4417, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1185, 1.4417, 0.0), glm::dvec3(-0.1026, 1.4417, -0.0603),
      glm::dvec3(-0.1571, 1.5333, -0.0924), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1026, 1.4417, -0.0603),
                                       glm::dvec3(-0.1185, 1.4417, 0.0),
                                       glm::dvec3(-0.1, 1.35, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1, 1.35, 0.0), glm::dvec3(-0.0865, 1.35, -0.0508),
      glm::dvec3(-0.1026, 1.4417, -0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(-0.1571, 1.5333, 0.0924), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1571, 1.5333, 0.0924),
                                       glm::dvec3(-0.1815, 1.5333, 0.0),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1815, 1.5333, 0.0), glm::dvec3(-0.1571, 1.5333, 0.0924),
      glm::dvec3(-0.1026, 1.4417, 0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1026, 1.4417, 0.0603), glm::dvec3(-0.1185, 1.4417, 0.0),
      glm::dvec3(-0.1815, 1.5333, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1185, 1.4417, 0.0), glm::dvec3(-0.1026, 1.4417, 0.0603),
      glm::dvec3(-0.0865, 1.35, 0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0865, 1.35, 0.0508), glm::dvec3(-0.1, 1.35, 0.0),
      glm::dvec3(-0.1185, 1.4417, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(-0.0924, 1.5333, 0.1571), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.0924, 1.5333, 0.1571),
                                       glm::dvec3(-0.1571, 1.5333, 0.0924),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1571, 1.5333, 0.0924), glm::dvec3(-0.0924, 1.5333, 0.1571),
      glm::dvec3(-0.0603, 1.4417, 0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0603, 1.4417, 0.1026), glm::dvec3(-0.1026, 1.4417, 0.0603),
      glm::dvec3(-0.1571, 1.5333, 0.0924), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1026, 1.4417, 0.0603), glm::dvec3(-0.0603, 1.4417, 0.1026),
      glm::dvec3(-0.0508, 1.35, 0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0508, 1.35, 0.0865), glm::dvec3(-0.0865, 1.35, 0.0508),
      glm::dvec3(-0.1026, 1.4417, 0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.0, 1.5333, 0.1815), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0, 1.5333, 0.1815),
                                       glm::dvec3(-0.0924, 1.5333, 0.1571),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0924, 1.5333, 0.1571), glm::dvec3(0.0, 1.5333, 0.1815),
      glm::dvec3(0.0, 1.4417, 0.1185), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.4417, 0.1185), glm::dvec3(-0.0603, 1.4417, 0.1026),
      glm::dvec3(-0.0924, 1.5333, 0.1571), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.0603, 1.4417, 0.1026),
                                       glm::dvec3(0.0, 1.4417, 0.1185),
                                       glm::dvec3(0.0, 1.35, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.35, 0.1), glm::dvec3(-0.0508, 1.35, 0.0865),
      glm::dvec3(-0.0603, 1.4417, 0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.0924, 1.5333, 0.1571), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.0924, 1.5333, 0.1571),
                                       glm::dvec3(0.0, 1.5333, 0.1815),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.5333, 0.1815), glm::dvec3(0.0924, 1.5333, 0.1571),
      glm::dvec3(0.0603, 1.4417, 0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0603, 1.4417, 0.1026), glm::dvec3(0.0, 1.4417, 0.1185),
      glm::dvec3(0.0, 1.5333, 0.1815), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.4417, 0.1185), glm::dvec3(0.0603, 1.4417, 0.1026),
      glm::dvec3(0.0508, 1.35, 0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0508, 1.35, 0.0865), glm::dvec3(0.0, 1.35, 0.1),
      glm::dvec3(0.0, 1.4417, 0.1185), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.1571, 1.5333, 0.0924), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.1571, 1.5333, 0.0924),
                                       glm::dvec3(0.0924, 1.5333, 0.1571),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0924, 1.5333, 0.1571), glm::dvec3(0.1571, 1.5333, 0.0924),
      glm::dvec3(0.1026, 1.4417, 0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1026, 1.4417, 0.0603), glm::dvec3(0.0603, 1.4417, 0.1026),
      glm::dvec3(0.0924, 1.5333, 0.1571), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0603, 1.4417, 0.1026), glm::dvec3(0.1026, 1.4417, 0.0603),
      glm::dvec3(0.0865, 1.35, 0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0865, 1.35, 0.0508), glm::dvec3(0.0508, 1.35, 0.0865),
      glm::dvec3(0.0603, 1.4417, 0.1026), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.575, 0.0), glm::dvec3(0.0, 1.575, 0.0),
      glm::dvec3(0.1815, 1.5333, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.1815, 1.5333, 0.0),
                                       glm::dvec3(0.1571, 1.5333, 0.0924),
                                       glm::dvec3(0.0, 1.575, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1571, 1.5333, 0.0924), glm::dvec3(0.1815, 1.5333, 0.0),
      glm::dvec3(0.1185, 1.4417, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1185, 1.4417, 0.0), glm::dvec3(0.1026, 1.4417, 0.0603),
      glm::dvec3(0.1571, 1.5333, 0.0924), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.1026, 1.4417, 0.0603),
                                       glm::dvec3(0.1185, 1.4417, 0.0),
                                       glm::dvec3(0.1, 1.35, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1, 1.35, 0.0), glm::dvec3(0.0865, 1.35, 0.0508),
      glm::dvec3(0.1026, 1.4417, 0.0603), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1, 1.35, 0.0), glm::dvec3(0.0865, 1.35, -0.0508),
      glm::dvec3(0.2483, 1.2944, -0.1459), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.2483, 1.2944, -0.1459),
                                       glm::dvec3(0.287, 1.2944, 0.0),
                                       glm::dvec3(0.1, 1.35, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.287, 1.2944, 0.0), glm::dvec3(0.2483, 1.2944, -0.1459),
      glm::dvec3(0.4582, 1.2556, -0.2691), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4582, 1.2556, -0.2691), glm::dvec3(0.5296, 1.2556, 0.0),
      glm::dvec3(0.287, 1.2944, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5296, 1.2556, 0.0), glm::dvec3(0.4582, 1.2556, -0.2691),
      glm::dvec3(0.5624, 1.2, -0.3303), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5624, 1.2, -0.3303), glm::dvec3(0.65, 1.2, 0.0),
      glm::dvec3(0.5296, 1.2556, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0865, 1.35, -0.0508), glm::dvec3(0.0508, 1.35, -0.0865),
      glm::dvec3(0.1459, 1.2944, -0.2483), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1459, 1.2944, -0.2483), glm::dvec3(0.2483, 1.2944, -0.1459),
      glm::dvec3(0.0865, 1.35, -0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2483, 1.2944, -0.1459), glm::dvec3(0.1459, 1.2944, -0.2483),
      glm::dvec3(0.2691, 1.2556, -0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2691, 1.2556, -0.4582), glm::dvec3(0.4582, 1.2556, -0.2691),
      glm::dvec3(0.2483, 1.2944, -0.1459), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4582, 1.2556, -0.2691), glm::dvec3(0.2691, 1.2556, -0.4582),
      glm::dvec3(0.3303, 1.2, -0.5624), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3303, 1.2, -0.5624), glm::dvec3(0.5624, 1.2, -0.3303),
      glm::dvec3(0.4582, 1.2556, -0.2691), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0508, 1.35, -0.0865), glm::dvec3(0.0, 1.35, -0.1),
      glm::dvec3(0.0, 1.2944, -0.287), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2944, -0.287), glm::dvec3(0.1459, 1.2944, -0.2483),
      glm::dvec3(0.0508, 1.35, -0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1459, 1.2944, -0.2483), glm::dvec3(0.0, 1.2944, -0.287),
      glm::dvec3(0.0, 1.2556, -0.5296), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2556, -0.5296), glm::dvec3(0.2691, 1.2556, -0.4582),
      glm::dvec3(0.1459, 1.2944, -0.2483), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.2691, 1.2556, -0.4582),
                                       glm::dvec3(0.0, 1.2556, -0.5296),
                                       glm::dvec3(0.0, 1.2, -0.65), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, -0.65), glm::dvec3(0.3303, 1.2, -0.5624),
      glm::dvec3(0.2691, 1.2556, -0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.35, -0.1), glm::dvec3(-0.0508, 1.35, -0.0865),
      glm::dvec3(-0.1459, 1.2944, -0.2483), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1459, 1.2944, -0.2483),
                                       glm::dvec3(0.0, 1.2944, -0.287),
                                       glm::dvec3(0.0, 1.35, -0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2944, -0.287), glm::dvec3(-0.1459, 1.2944, -0.2483),
      glm::dvec3(-0.2691, 1.2556, -0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.2691, 1.2556, -0.4582), glm::dvec3(0.0, 1.2556, -0.5296),
      glm::dvec3(0.0, 1.2944, -0.287), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2556, -0.5296), glm::dvec3(-0.2691, 1.2556, -0.4582),
      glm::dvec3(-0.3303, 1.2, -0.5624), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3303, 1.2, -0.5624), glm::dvec3(0.0, 1.2, -0.65),
      glm::dvec3(0.0, 1.2556, -0.5296), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0508, 1.35, -0.0865), glm::dvec3(-0.0865, 1.35, -0.0508),
      glm::dvec3(-0.2483, 1.2944, -0.1459), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.2483, 1.2944, -0.1459),
                                       glm::dvec3(-0.1459, 1.2944, -0.2483),
                                       glm::dvec3(-0.0508, 1.35, -0.0865),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.1459, 1.2944, -0.2483),
                                       glm::dvec3(-0.2483, 1.2944, -0.1459),
                                       glm::dvec3(-0.4582, 1.2556, -0.2691),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4582, 1.2556, -0.2691),
                                       glm::dvec3(-0.2691, 1.2556, -0.4582),
                                       glm::dvec3(-0.1459, 1.2944, -0.2483),
                                       tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.2691, 1.2556, -0.4582),
                                       glm::dvec3(-0.4582, 1.2556, -0.2691),
                                       glm::dvec3(-0.5624, 1.2, -0.3303),
                                       tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5624, 1.2, -0.3303), glm::dvec3(-0.3303, 1.2, -0.5624),
      glm::dvec3(-0.2691, 1.2556, -0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0865, 1.35, -0.0508), glm::dvec3(-0.1, 1.35, 0.0),
      glm::dvec3(-0.287, 1.2944, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.287, 1.2944, 0.0), glm::dvec3(-0.2483, 1.2944, -0.1459),
      glm::dvec3(-0.0865, 1.35, -0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.2483, 1.2944, -0.1459), glm::dvec3(-0.287, 1.2944, 0.0),
      glm::dvec3(-0.5296, 1.2556, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5296, 1.2556, 0.0), glm::dvec3(-0.4582, 1.2556, -0.2691),
      glm::dvec3(-0.2483, 1.2944, -0.1459), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.4582, 1.2556, -0.2691),
                                       glm::dvec3(-0.5296, 1.2556, 0.0),
                                       glm::dvec3(-0.65, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.65, 1.2, 0.0), glm::dvec3(-0.5624, 1.2, -0.3303),
      glm::dvec3(-0.4582, 1.2556, -0.2691), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1, 1.35, 0.0), glm::dvec3(-0.0865, 1.35, 0.0508),
      glm::dvec3(-0.2483, 1.2944, 0.1459), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.2483, 1.2944, 0.1459),
                                       glm::dvec3(-0.287, 1.2944, 0.0),
                                       glm::dvec3(-0.1, 1.35, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.287, 1.2944, 0.0), glm::dvec3(-0.2483, 1.2944, 0.1459),
      glm::dvec3(-0.4582, 1.2556, 0.2691), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4582, 1.2556, 0.2691), glm::dvec3(-0.5296, 1.2556, 0.0),
      glm::dvec3(-0.287, 1.2944, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5296, 1.2556, 0.0), glm::dvec3(-0.4582, 1.2556, 0.2691),
      glm::dvec3(-0.5624, 1.2, 0.3303), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.5624, 1.2, 0.3303), glm::dvec3(-0.65, 1.2, 0.0),
      glm::dvec3(-0.5296, 1.2556, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0865, 1.35, 0.0508), glm::dvec3(-0.0508, 1.35, 0.0865),
      glm::dvec3(-0.1459, 1.2944, 0.2483), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1459, 1.2944, 0.2483), glm::dvec3(-0.2483, 1.2944, 0.1459),
      glm::dvec3(-0.0865, 1.35, 0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.2483, 1.2944, 0.1459), glm::dvec3(-0.1459, 1.2944, 0.2483),
      glm::dvec3(-0.2691, 1.2556, 0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.2691, 1.2556, 0.4582), glm::dvec3(-0.4582, 1.2556, 0.2691),
      glm::dvec3(-0.2483, 1.2944, 0.1459), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.4582, 1.2556, 0.2691), glm::dvec3(-0.2691, 1.2556, 0.4582),
      glm::dvec3(-0.3303, 1.2, 0.5624), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.3303, 1.2, 0.5624), glm::dvec3(-0.5624, 1.2, 0.3303),
      glm::dvec3(-0.4582, 1.2556, 0.2691), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.0508, 1.35, 0.0865), glm::dvec3(0.0, 1.35, 0.1),
      glm::dvec3(0.0, 1.2944, 0.287), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2944, 0.287), glm::dvec3(-0.1459, 1.2944, 0.2483),
      glm::dvec3(-0.0508, 1.35, 0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(-0.1459, 1.2944, 0.2483), glm::dvec3(0.0, 1.2944, 0.287),
      glm::dvec3(0.0, 1.2556, 0.5296), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2556, 0.5296), glm::dvec3(-0.2691, 1.2556, 0.4582),
      glm::dvec3(-0.1459, 1.2944, 0.2483), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(-0.2691, 1.2556, 0.4582),
                                       glm::dvec3(0.0, 1.2556, 0.5296),
                                       glm::dvec3(0.0, 1.2, 0.65), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2, 0.65), glm::dvec3(-0.3303, 1.2, 0.5624),
      glm::dvec3(-0.2691, 1.2556, 0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.35, 0.1), glm::dvec3(0.0508, 1.35, 0.0865),
      glm::dvec3(0.1459, 1.2944, 0.2483), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.1459, 1.2944, 0.2483),
                                       glm::dvec3(0.0, 1.2944, 0.287),
                                       glm::dvec3(0.0, 1.35, 0.1), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2944, 0.287), glm::dvec3(0.1459, 1.2944, 0.2483),
      glm::dvec3(0.2691, 1.2556, 0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2691, 1.2556, 0.4582), glm::dvec3(0.0, 1.2556, 0.5296),
      glm::dvec3(0.0, 1.2944, 0.287), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0, 1.2556, 0.5296), glm::dvec3(0.2691, 1.2556, 0.4582),
      glm::dvec3(0.3303, 1.2, 0.5624), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.3303, 1.2, 0.5624), glm::dvec3(0.0, 1.2, 0.65),
      glm::dvec3(0.0, 1.2556, 0.5296), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0508, 1.35, 0.0865), glm::dvec3(0.0865, 1.35, 0.0508),
      glm::dvec3(0.2483, 1.2944, 0.1459), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2483, 1.2944, 0.1459), glm::dvec3(0.1459, 1.2944, 0.2483),
      glm::dvec3(0.0508, 1.35, 0.0865), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.1459, 1.2944, 0.2483), glm::dvec3(0.2483, 1.2944, 0.1459),
      glm::dvec3(0.4582, 1.2556, 0.2691), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.4582, 1.2556, 0.2691), glm::dvec3(0.2691, 1.2556, 0.4582),
      glm::dvec3(0.1459, 1.2944, 0.2483), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2691, 1.2556, 0.4582), glm::dvec3(0.4582, 1.2556, 0.2691),
      glm::dvec3(0.5624, 1.2, 0.3303), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5624, 1.2, 0.3303), glm::dvec3(0.3303, 1.2, 0.5624),
      glm::dvec3(0.2691, 1.2556, 0.4582), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.0865, 1.35, 0.0508), glm::dvec3(0.1, 1.35, 0.0),
      glm::dvec3(0.287, 1.2944, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.287, 1.2944, 0.0), glm::dvec3(0.2483, 1.2944, 0.1459),
      glm::dvec3(0.0865, 1.35, 0.0508), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.2483, 1.2944, 0.1459), glm::dvec3(0.287, 1.2944, 0.0),
      glm::dvec3(0.5296, 1.2556, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.5296, 1.2556, 0.0), glm::dvec3(0.4582, 1.2556, 0.2691),
      glm::dvec3(0.2483, 1.2944, 0.1459), tea_pot));
  world.add(std::make_shared<triangle>(glm::dvec3(0.4582, 1.2556, 0.2691),
                                       glm::dvec3(0.5296, 1.2556, 0.0),
                                       glm::dvec3(0.65, 1.2, 0.0), tea_pot));
  world.add(std::make_shared<triangle>(
      glm::dvec3(0.65, 1.2, 0.0), glm::dvec3(0.5624, 1.2, 0.3303),
      glm::dvec3(0.4582, 1.2556, 0.2691), tea_pot));
}
