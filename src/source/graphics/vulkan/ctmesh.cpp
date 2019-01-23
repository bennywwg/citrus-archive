#include <graphics/vulkan/ctmesh.h>

namespace citrus::graphics {
	bool ctmesh::hasColor() const {
		return !color.empty();
	}
	bool ctmesh::hasNorm() const {
		return !norm.empty();
	}
	bool ctmesh::hasTangent() const {
		return !tangent.empty();
	}
	bool ctmesh::hasUV() const {
		return !uv.empty();
	}
	bool ctmesh::hasBones() const {
		return !bone0.empty();
	}

	uint64_t ctmesh::vertSizeWithoutPadding() const {
		uint64_t vertSize = sizeof(vec3);
		if(hasColor()) vertSize += sizeof(vec3);
		if(hasNorm()) vertSize += sizeof(vec3);
		if(hasTangent()) vertSize += sizeof(vec3);
		if(hasUV()) vertSize += sizeof(vec2);
		if(hasBones()) vertSize += (sizeof(int) + sizeof(int) + sizeof(float) + sizeof(float));
		return vertSize;
	}
	uint64_t ctmesh::requiredMemory() const {
		return pos.size() * vertSizeWithoutPadding();
	}
	void ctmesh::construct(void* vdata) const {
		uint8_t* data = (uint8_t*)vdata;
		uint64_t stride = vertSizeWithoutPadding();
		uint64_t voffset = 0;
		for(int i = 0; i < pos.size(); i++)
			*(vec3*)(data + i * stride) = pos[i];
		voffset += sizeof(vec3);

		if(hasColor()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec3*)(data + i * stride + voffset) = color[i];
			voffset += sizeof(vec3);
		}

		if(hasNorm()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec3*)(data + i * stride + voffset) = norm[i];
			voffset += sizeof(vec3);
		}

		if(hasTangent()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec3*)(data + i * stride + voffset) = tangent[i];
			voffset += sizeof(vec3);
		}

		if(hasUV()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec2*)(data + i * stride + voffset) = uv[i];
			voffset += sizeof(vec2);
		}

		if(hasBones()) {
			for(int i = 0; i < pos.size(); i++) {
				*(int32_t*)(data + i * stride) = bone0[i];
			}
			voffset += sizeof(int);

			for(int i = 0; i < pos.size(); i++) {
				*(int32_t*)(data + i * stride) = bone1[i];
			}
			voffset += sizeof(int);

			for(int i = 0; i < pos.size(); i++) {
				*(float*)(data + i * stride) = weight0[i];
			}
			voffset += sizeof(float);

			for(int i = 0; i < pos.size(); i++) {
				*(float*)(data + i * stride) = weight1[i];
			}
			voffset += sizeof(float);
		}
	}
}