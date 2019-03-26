#pragma once

#ifndef GEOM_H
#define GEOM_H

#include "citrus/util.h"
#include <iostream>

namespace citrus::geom {
	class line {
		public:
		glm::vec3 p0, p1;
		line(glm::vec3 p0, glm::vec3 p1) : p0(p0), p1(p1) { }
	};

	
	struct ray {
		vec3 p, d;
	};

	struct hit {
	private:
		bool _hit;
		float _dist;
	public:
		float dist() {
			return _dist;
		}
		operator bool() const {
			return _hit;
		}
		hit(bool hit, float dist) : _hit(hit), _dist(dist) { }
	};

	struct cylinder {
		vec3 p0, p1;
		float r;
		inline hit intersections(const ray& ray) {
			//(
		}
	};

	struct sphere {
		vec3 p;
		float r;
		inline hit intersects(const ray& ray) {
			//x = r.d.x * t + r.p.x;
			//(r.d.x * t + r.p.x - p.x)^2 + (r.d.y * t + r.p.y - p.y)^2 + (r.d.z * t + r.p.z - p.z)^2 = r^2
			//(r.d.x * t + (r.p.x - p.x))^2 + (r.d.y * t + (r.p.y - p.y))^2 + (r.d.z * t + (r.p.z - p.z))^2 = r^2
			//(r.d.x^2 * t^2 + 2*(r.d.x)*(r.p.x-p.x)*t + (r.p.x-p.x)^2 + ... + (r.d.z^2 * t^2 + 2*(r.d.z)*(r.p.z-p.z)*t + (r.p.z-p.z)^2 = r^2
			//a = r.d.x^2 + r.d.y^2 + r.d.z^2
			//b = 2*((r.d.x)*(r.p.x-p.x) + (r.d.y)*(r.p.y-p.y) + (r.d.z)*(r.p.z-p.z))
			//c = (r.p.x-p.x)^2 + (r.p.y-p.y)^2 + (r.p.z-p.z)^2
			//t = -(b + sqrt(b^2 - 4*a*c)) / (2*a)
			
			/*float a = ray.d.x * ray.d.x + ray.d.y * ray.d.y + ray.d.z * ray.d.z;
			float b = 2 * ((ray.d.x)*(ray.p.x - p.x) + (ray.d.y)*(ray.p.y - p.y) + (ray.d.z)*(ray.p.z - p.z));
			float c = pow(ray.p.x - p.x, 2) + pow(ray.p.y - p.y, 2) + pow(ray.p.z - p.z, 2) - r * r;
			float d = b * b - 4 * a * c;
			if(d <= 0) return hit(false, 0.0f);
			float e = -0.5f * (b + sqrt(d)) / a;
			float f = -0.5f * (b - sqrt(d)) / a;
			hit res(true, e >= 0 ? e : f);*/

			vec3 r_c = p - ray.p;
			vec3 rdn = glm::normalize(ray.d);
			float dist = glm::dot(rdn, r_c);
			if(dist >= 0) {
				return hit(glm::length2(ray.p + rdn * dist - p) <= r * r, dist);
			} else {
				return hit(false, 0.0f);
			}
		}
	};

	struct triangle {
		vec3 p0, p1, p2;
		hit intersection(const ray& ray, vec3& outIntersectionPoint) {
			const float EPSILON = 0.0000001f;
			vec3 vertex0 = p0;
			vec3 vertex1 = p1;
			vec3 vertex2 = p2;
			vec3 edge1, edge2, h, s, q;
			float a, f, u, v;
			edge1 = vertex1 - vertex0;
			edge2 = vertex2 - vertex0;
			h = glm::cross(ray.d, edge2);
			a = glm::dot(edge1, h);
			if(a > -EPSILON && a < EPSILON)
				return hit(false, 0.0f);    // This ray is parallel to this triangle.
			f = 1.0 / a;
			s = ray.p - vertex0;
			u = f * glm::dot(s, h);
			if(u < 0.0 || u > 1.0)
				return hit(false, 0.0f);
			q = glm::cross(s, edge1);
			v = f * glm::dot(ray.d, q);
			if(v < 0.0 || u + v > 1.0)
				return hit(false, 0.0f);
			// At this stage we can compute t to find out where the intersection point is on the line.
			float t = f * glm::dot(edge2, q);
			if(t > EPSILON) // ray intersection
			{
				outIntersectionPoint = ray.p + ray.d * t;
				return hit(true, t);
			} else // This means that there is a line intersection but not a ray intersection.
				return hit(false, 0.0f);
		}
	};

}

#endif
