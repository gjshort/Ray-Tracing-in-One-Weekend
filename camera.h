#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

//Construct and dispatch rays into the world.
//Use the results of these rays to construct the rendered image.

class camera {
private:
	int image_height;	//Rendered image height
	double pixel_samples_scale;	//Color scale factor for a sum of pixels
	point3 camera_center;		//Camera center
	point3 pixel00_loc; //Location of upper left pixel 0,0 (not viewport corner)
	vec3 pixel_delta_u;	//Horizontal pixel spacing vector
	vec3 pixel_delta_v;	//Vertical pixel spacing vector
	vec3 u, v, w; //Camera frame basis vectors
	vec3 defocus_disk_u;
	vec3 defocus_disk_v; //Defocus radii
	
	void initialize() {
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height > 1) ? image_height : 1;

		pixel_samples_scale = 1.0 / samples_per_pixel;

		camera_center = lookfrom; // Camera at lookfrom point

		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2);
		auto viewport_height = 2 * h * focus_dist;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		//Camera orientation basis vectors
		w = unit_vector(lookfrom - lookat);	//Opposite viewing direction
		u = unit_vector(cross(vup, w)); //Camera right
		v = unit_vector(cross(w, u)); //Camera up

		vec3 viewport_u = viewport_width * u; // Horizontal viewport vector from top left
		vec3 viewport_v = viewport_height * -v; // Vertical "..." (points down)

		//Distance between pixels (delta_)
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		//Location of top left pixel (First we need location of top left corner, then offset by delta distance)
		auto viewport_upper_left = camera_center - (focus_dist * w) - (viewport_u / 2) - (viewport_v / 2);
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	
		//Calculate defocus disk behavior
		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	color ray_color(const ray& r, int depth, const hittable& world) const {
		if (depth <= 0) {
			return color(0, 0, 0);
		}
		
		hit_record rec;
		if (world.hit(r, interval(0.001, infinity), rec)) {
			ray scattered;
			color attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered)) {
				return attenuation * ray_color(scattered, depth - 1, world);
			}
			return color(0, 0, 0);
		}

		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}

	// Construct a camera ray originating from the defocus disk and directed at a randomly
	// sampled point around the pixel location i, j.
	ray get_ray(int i, int j) const {
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc
			+ ((i + offset.x()) * pixel_delta_u)
			+ ((j + offset.y()) * pixel_delta_v);

		auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}
	
	vec3 sample_square() const {
		return vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}

	point3 defocus_disk_sample() const {
		auto p = random_in_unit_disk();
		return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}
	
public:
	// Image
	double aspect_ratio = 1.0; //1x1 aspect ratio
	int image_width = 100;	//100 pixel width
	int samples_per_pixel = 10;	//Count of random samples for each pixel (anti-aliasing)
	int max_depth = 10; //Maximum number of child rays for reflection

	double vfov = 90; //90* vertical FOV
	point3 lookfrom = point3(0, 0, 0);
	point3 lookat = point3(0, 0, -1);
	vec3 vup = vec3(0, 1, 0); //Vertical camera direction

	double defocus_angle = 0;
	double focus_dist = 10;

	//Render objects in world
	void render(const hittable& world) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			
			for (int i = 0; i < image_width; i++) {
				color pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i,j);
					pixel_color += ray_color(r, max_depth, world);
				}

				write_color(std::cout, pixel_samples_scale * pixel_color);
			}
		}

		std::clog << "\rDone.                   \n";
	}


};

#endif