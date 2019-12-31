#pragma once

#define CT_USE_EDITOR

#ifdef CT_USE_EDITOR
#include "util.h"

namespace citrus {
	struct grouping {
		mat4			tr = glm::identity<mat4>();
		vec3			color = vec3(1.0f, 1.0f, 1.0f);
		std::vector<vec3>	data;
		std::vector<vec2>	uvdata;
		std::vector<vec3>	altData;
		bool			pixelspace = false;
		/// add a mesh presenting a string to this grouping
		/// fills in any missing uv data if needed
		void addText(std::string text, int px, ivec2 pos = ivec2(0, 0));
		void addCube(vec3 halfDims);
		void addSphere(float rad, int thetaCount, int phiCount);
		void addTorus(float radMajor, float radMinor, uint32_t majSegs, uint32_t minSegs);
		void addArrow(float rad, float len, uint32_t majorCount);
	};
}
#endif