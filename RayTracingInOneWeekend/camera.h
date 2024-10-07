#pragma once

#include <iostream>
#include <fstream>

#include "rtweekend.h"
#include "colour.h"
#include "hittable.h"
#include "material.h"

class camera {
public:
	double aspect_ratio = 1.0;
	int image_width = 100;
	int samples_per_pixel = 10;
	int max_depth = 10;

	double vfov = 90;
	point3 lookfrom = point3(0, 0, -1);
	point3 lookat = point3(0, 0, 0);
	vec3 vup = vec3(0, 1, 0);

	double defocus_angle = 0;
	double focus_dist = 10;

	void render(const hittable& world) {
		initialise();

		ofstream of("image.ppm");

		of << "P3" << endl << image_width << ' ' << image_height << endl << "255" << endl;
		for (int j = 0; j < image_height; j++) {
			clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;
			for (int i = 0; i < image_width; i++) {
				colour pixel_colour(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_colour += ray_colour(r, max_depth, world);
				}
				write_colour(of, pixel_colour, samples_per_pixel);
			}
		}

		clog << "\n Done";
	}
private:
	int image_height;
	point3 camera_center;
	point3 pixel00_loc;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;
	vec3 u, v, w;
	vec3 defocus_disk_u;
	vec3 defocus_disk_v;

	void initialise() {
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		camera_center = lookfrom;

		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2* h * focus_dist;
		auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_height * -v;

		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		auto viewport_upper_left = camera_center - (focus_dist*w) - (viewport_u / 2) - (viewport_v / 2);
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

	}

	ray get_ray(int i, int j) const {
		auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		auto pixel_sample = pixel_center + pixel_sample_square();

		auto ray_origin = (defocus_angle <=0) ? camera_center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	point3 defocus_disk_sample() const {
		auto p = random_in_unit_disk();
		return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	vec3 pixel_sample_square() const {
		auto px = -0.5 + random_double();
		auto py = -0.5 + random_double();
		return(px * pixel_delta_u) + (py * pixel_delta_v);
	}

	colour ray_colour(const ray& r, int depth, const hittable& world) const {
		hit_record rec;

		if (depth <= 0) {
			return colour(0, 0, 0);
		}

		if (world.hit(r, interval(0.001, infinity), rec)) {
			ray scattered;
			colour attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered)) {
				return attenuation * ray_colour(scattered,depth-1,world);	
			}
			return colour(0, 0, 0);
		}
				
		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0);
	}
};
