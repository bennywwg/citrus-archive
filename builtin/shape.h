#pragma once

#include "../mankern/util.h"

namespace citrus {
	enum class shapeType : int {
		none = 0,
		sphere = 1,
		box = 2,
		hull = 3,
		triangleMesh = 4, // static only
		capsule = 5,
	};

	class shape {
	public:
		shapeType type;

		vec3 state;

		std::vector<vec3> verts;
		std::vector<int> indices;

		inline shape() : type(shapeType::none), state(1.f, 1.f, 1.f) { }
	};
}