#include <opencv2/opencv.hpp>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <glm/glm.hpp>
#include "rtweek.h"

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "defuse_light.h"
#include "hittable_list.h"

#include "triangle.h"
#include "sphere.h"
#include "inf_plane.h"

void cWindow(cv::String windowName);
void render();
void renderLoop(int name);
void dWindow(cv::String windowName);

cv::String windowName = "Ray Tracing";
cv::Mat img(image_height, image_width, CV_64FC3, cv::Scalar(0, 0, 0));
mycamera cam(point3(0, 0, 0));
hittable_list world;

int thread;
int samples_per_pixel;
int printN;

inline cv::Vec3d ray_color(const ray &r, const hittable &world, int depth)
{
	hit_record rec;

	if (depth <= 0)
	{
		return cv::Vec3d(0, 0, 0);
	}

	if (!world.hit(r, 0.0001, infinity, rec))
		return cv::Vec3d(0, 0, 0);

	ray scattered;
	cv::Vec3d attenuation;
	cv::Vec3d emitted = rec.mat_ptr->emitted();

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation.mul(ray_color(scattered, world, depth - 1));

	// cv::Vec3d unit_direction = cv::normalize(r.directionOPEN());
	// auto hight = 0.5 * (unit_direction[1] + 1.0);
	// return (1.0 - hight) * cv::Vec3d(1.0, 1.0, 1.0) + hight * cv::Vec3d(0.5, 0.7, 1.0);
}

int main(int argc, char **argv)
{
	thread = std::atoi(argv[1]);
	printN = std::atoi(argv[3]);
	samples_per_pixel = std::atoi(argv[2]);
	cWindow(windowName);

	render();

	// cv::imshow(windowName, img / (samples_per_pixel * thread)); // Show our image inside the created window.

	img.convertTo(img, CV_8U, 255 / ((float)samples_per_pixel * thread), 0);

	cv::Mat lookUpTable(1, 256, CV_8U);
	uchar *p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, 0.5) * 255.0);
	cv::Mat res = img.clone();
	LUT(img, lookUpTable, res);

	cv::imwrite("render-img.png", img);
	cv::imwrite("render-res.png", res);

	// dWindow(windowName);
	return 0;
}

void render()
{

	// Texture
	auto material_sphere = make_shared<defuse>(cv::Vec3d(1, 1, 0));
	auto material_ground = make_shared<defuse>(cv::Vec3d(0.1, 0.1, 0.1));
	auto material_light = make_shared<defuse_light>(cv::Vec3d(1, 1, 1) * 1);

	// Object
	world.add(make_shared<sphere>(cv::Vec3d(0, -1000.25, 0), 1000, material_ground));

	world.add(make_shared<sphere>(cv::Vec3d(0, 5, 0), 2, material_light));
	world.add(make_shared<sphere>(cv::Vec3d(0, 0, -1), 0.24, material_sphere));

	if (thread)
	{
		std::vector<std::thread> workingThread;
		for (int i = 1; i <= thread; i++)
		{
			workingThread.push_back(std::thread(renderLoop, i));
		}
		for (int i = 0; i < thread; i++)
		{
			workingThread[i].join();
		}
	}
	else
	{
		std::thread t1(renderLoop, 1);
		t1.join();
	}
}

std::mutex m_image;
void renderLoop(int name)
{
	thread_local cv::Mat t_img(image_height, image_width, CV_64FC3, cv::Scalar(0, 0, 0));
	thread_local int sampleN = 0;

	while (sampleN++ < samples_per_pixel)
	{
		for (int i = 0; i < image_height; ++i)
		{
			for (int j = 0; j < image_width; ++j)
			{
				auto u = (j + random_double());
				auto v = (i + random_double());
				ray r = cam.get_ray(u, v);

				cv::Vec3d pixel_color = ray_color(r, world, max_depth);
				cv::Vec3d &point = t_img.at<cv::Vec3d>(i, j);

				addPixelColor(point, pixel_color);
			}
		}
		if (!(sampleN % printN))
		{
			std::cout << "\nRemaining time for " << name << ": " << sampleN << '/' << samples_per_pixel << "\t" << std::flush;
			m_image.lock();
			img += t_img;
			cv::imshow(windowName, img / (sampleN * thread)); // Show our image inside the created window.
			cv::waitKey(5);
			m_image.unlock();
		}
	}
}

void cWindow(cv::String windowName)
{
	namedWindow(windowName, cv::WINDOW_NORMAL); // Create a window
}

void dWindow(cv::String windowName)
{
	char k;
	while (cv::waitKey(0) != ' ')
		;
	cv::destroyWindow(windowName); // destroy the created window
}