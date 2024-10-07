#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

using namespace std;

//constats

const double infinity = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//utility functions

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

//common headers
#include "ray.h"
#include "vec3.h"
#include "interval.h"
