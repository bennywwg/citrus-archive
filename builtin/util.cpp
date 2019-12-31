#include "util.h"

namespace citrus {
	vec3 btToGlm(btVector3 vec) {
		return vec3(vec.getX(), vec.getY(), vec.getZ());
	}
	btVector3 glmToBt(vec3 vec) {
		return btVector3(vec.x, vec.y, vec.z);
	}
	transform btToGlm(btTransform tr) {
		return transform(btToGlm(tr.getOrigin()), btToGlm(tr.getRotation()));
	}

	quat btToGlm(btQuaternion ori) {
		return quat(ori.getW(), ori.getX(), ori.getY(), ori.getZ());
	}
	btQuaternion glmToBt(quat quat) {
		return btQuaternion(quat.x, quat.y, quat.z, quat.w);
	}
	btTransform glmToBt(transform tr) {
		return btTransform(glmToBt(tr.getOrientation()), glmToBt(tr.getPosition()));
	}
	void cappedCylinder_py(int segments, float rad, float len, mat4 tr, std::vector<vec3> &v) {
		for (int i = 0; i < segments; i++) {
			float ama0 = float(i) / float(segments) * glm::pi<float>() * 2.0f;
			float ama1 = float(i + 1) / float(segments) * glm::pi<float>() * 2.0f;
			float cos0 = glm::cos(ama0) * rad;
			float sin0 = glm::cos(ama1) * rad;
			float cos1 = glm::cos(ama1) * rad;
			float sin1 = glm::cos(ama1) * rad;
			v.emplace_back(cos0, 0, sin0);
			v.emplace_back(cos1, 0, sin1);
			v.emplace_back(cos0, len, sin0);
			v.emplace_back(cos0, len, sin0);
			v.emplace_back(cos1, 0, sin1);
			v.emplace_back(cos1, len, sin1);
		}
		for (vec3& vec : v) {
			vec = vec3(tr * vec4(vec, 1.0f));
		}
	}
}