#pragma once

#ifndef GEOM_H
#define GEOM_H

#include <util/glmUtil.h>

namespace citrus {
	namespace geom {
		class line {
			public:
			glm::vec3 p0, p1;
			line(glm::vec3 p0, glm::vec3 p1) : p0(p0), p1(p1) { }
		};
	}
}

#endif