#ifndef VEC3_H
#define VEC3_H

//3D vector able to hold RGB, XYZ, etc

class vec3 {
public:
	double e[3];

	vec3() : e{ 0,0,0 } {} //Deafult constructor
	vec3(double e0, double e1, double e2) : e{ e0,e1,e2 } {} //Arg constructor
	//Member init list directly sets member value instead
	//of setting to default and then waiting for assignment in const. body

	//Getters (const indicates can't modify member data)
	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } // Take the negative of the vector
	double operator[](int i) const { return e[i]; } // Retrieve the indexed coord
	double& operator[](int i) { return e[i]; } // Ref. to indexed coord

	//Vector addition
	vec3& operator+=(const vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	//Constant multiplication
	vec3& operator*=(double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	//Constant division
	vec3& operator/=(double t) {
		return *this *= (1 / t); 
	}

	//3D Pythagorean Theorem for length of 3D Vector
	// sqrt(x^2 + y^2 + z^2)
	double length() const {
		return std::sqrt(length_squared());
	}
	double length_squared() const {
		return (e[0])*(e[0]) + (e[1])*(e[1]) + (e[2])*(e[2]);
	}

	bool near_zero() const {
		// Return true if the vector is close to zero in all dimensions.
		auto s = 1e-8;
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
	}

	static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

};

using point3 = vec3; //Alias for vec3 to specify the implementation of a 3D coord

//Vector utility functions

//Printing a vector
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

//Algebraic sum of vectors
inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

//Algebraic subtraction of vectors
inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

//Algebraic multiplication of vectors
inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

//Constant multiplication of vectors t * v
inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

//Constant multiplication of vectors v * t
inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

//Constant division of vectors
inline vec3 operator/(const vec3& v, double t) {
	return (1 / t) * v;
}

//Dot product of vectors
inline double dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

//Cross product of vectors
inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

//Unit vector (divide all dimensions by magnitude, will then all sum to mag of 1)
inline vec3 unit_vector(const vec3& v) {
	return v / v.length();
}

//Generate a random point within the unit circle
inline vec3 random_in_unit_disk() {
	while (true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() < 1)
			return p;
	}
}

//Generates a random unit vector in the unit sphere for reflections
inline vec3 random_unit_vector() {
	while (true) {
		auto p = vec3::random(-1, 1);
		auto len_sqrd = p.length_squared();
		if (1e-160 < len_sqrd && len_sqrd <= 1) {
			return p / sqrt(len_sqrd);
		}
	}
}

//Ensure reflected ray points out of the sphere
inline vec3 random_on_hemisphere(const vec3& normal) {
	vec3 on_unit_sphere = random_unit_vector();
	if (dot(normal, on_unit_sphere) > 0.0) {
		return on_unit_sphere;
	}
	else {
		return -on_unit_sphere;
	}
}

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

//Returns a refracted ray based on Snell's law
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = std::fmin(dot(-uv, n), 1.0);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_parallel = -std::sqrt(std::fabs(1 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}


#endif