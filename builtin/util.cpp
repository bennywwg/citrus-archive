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
}