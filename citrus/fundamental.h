#pragma once

#ifndef FUNDAMENTAL_H
#define FUNDAMENTAL_H

#include <glm\ext.hpp>

namespace citrus {
	namespace geom {
		struct line {
			glm::vec3 p0;
			glm::vec3 p1;

			const glm::vec3& operator[](int i) const {
				if (i % 2 == 0) {
					return p0;
				} else {
					return p1;
				}
			}
			glm::vec3& operator[](int i) {
				if (i % 2 == 0) {
					return p0;
				} else {
					return p1;
				}
			}

			line() {}
			line(glm::vec3 p0, glm::vec3 p1) : p0(p0), p1(p1) {}
		};
	}
}

#endif