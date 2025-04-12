#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
private:
	point3 center;
	double radius;
    shared_ptr<material> mat;
public:
    //Construct a sphere with specified center and radius (override w/ 0 if negative)
	sphere(const point3& center, double radius, shared_ptr<material> mat) : center(center), radius(std::fmax(0, radius)), mat(mat) {
        //Init material pointer 'mat'
    }
    
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        //Use the root within t bounds. If neither, return false.
        auto sqrtd = std::sqrt(discriminant);
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }
        
        rec.t = root;           //Save root to value of t that is hit
        rec.p = r.at(rec.t);    //Save hit point
        vec3 outward_normal = (rec.p - center) / radius; //Compute unit surface normal at point
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }
};

#endif
