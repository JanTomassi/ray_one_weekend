#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Image
const auto aspect_ratio = 6.0 / 4.0;
const int image_width = 640;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int max_depth = 8;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

inline double random_double()
{
    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(mt);
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

#endif