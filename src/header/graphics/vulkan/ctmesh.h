#pragma once

#include <memory>
#include <vector>
#include <util/glmUtil.h>

using std::vector;

namespace citrus::graphics {
	class ctmesh {
	public:
		vector<vec3> pos;
		vector<vec3> color;
		vector<vec3> norm;
		vector<vec3> tangent;
		vector<vec2> uv;
		vector<int> bone0;
		vector<int> bone1;
		vector<float> weight0;
		vector<float> weight1;

		bool hasColor() const;
		bool hasNorm() const;
		bool hasTangent() const;
		bool hasUV() const;
		bool hasBones() const;

		uint64_t vertSizeWithoutPadding() const;
		uint64_t requiredMemory() const;
		void construct(void* data) const;
	};
}