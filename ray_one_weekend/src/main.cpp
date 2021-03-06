//Uncomment the following line if you are compiling this code in Visual Studio
//#include "stdafx.h"

#include<opencv2/opencv.hpp>
#include<iostream>
#include "rtweek.h"

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "hittable_list.h"

using namespace std;
using namespace cv;

String windowName = "Ray Tracing";
void cWindow(String windowName);
void render(Mat image);
void dWindow(String windowName);

inline color ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;

	if (depth <= 0) {
		return color(0, 0, 0);
	}

	if (world.hit(r, 0.0001, infinity, rec)) {
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto hight = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - hight) * color(1.0, 1.0, 1.0) + hight * color(0.5, 0.7, 1.0);
}

int main(int argc, char** argv)
{

	cWindow(windowName);

	Mat img(image_height, image_width, CV_64FC3, Scalar(0, 0, 0));

	render(img);

	resize(img, img, Size(image_width, image_height));

	imshow(windowName, img / samples_per_pixel); // Show our image inside the created window.

	dWindow(windowName);

	return 0;
}

void render(Mat image) {

	//World

	//Texture
	auto material_center = make_shared<defuse>(color(1, 1, 1));
	auto material_ground = make_shared<defuse>(color(0.1, 0.1, 0.1));

	//Object
	hittable_list world;
	world.add(make_shared<sphere>(point3(-0.55, 0, -1), 0.25, material_center));
	world.add(make_shared<sphere>(point3(0, 0, -1), 0.25, material_center));
	world.add(make_shared<sphere>(point3(0.55, 0, -1), 0.25, material_center));
	world.add(make_shared<inf_plane>(point3(0, -0.33, 0), vec3(0, 1, 0), material_ground));

	// Camera
	mycamera cam(point3(0, 0, 0));

	//Render
	int pos = 0;
	for (int s = 0; s < samples_per_pixel; s++) {
		++pos;
		if ((s % 5) == 0) std::cerr << "\rScanlines remaining: " << ((double)pos / (samples_per_pixel)) * 100 << "      " << std::flush;
		for (int i = 0; i < image_height; ++i) {
			for (int j = 0; j < image_width; ++j) {
				auto u = (j + random_double()) / (image_width - 1);
				auto v = (i + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);

				color pixel_color = ray_color(r, world, max_depth);
				Vec3d& point = image.at<Vec3d>(i, j);

				addPixelColor(point, pixel_color, 1);
			}
		}
		imshow(windowName, image / s);
		waitKey(1);
	}
}

void cWindow(String windowName) {
	namedWindow(windowName); // Create a window
}

void dWindow(String windowName) {
	waitKey(0); // Wait for any keystroke in the window
	destroyWindow(windowName); //destroy the created window
}