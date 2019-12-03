#pragma once

#include <btBulletDynamicsCommon.h>

#include "../mankern/util.h"

namespace citrus {
	vec3 btToGlm(btVector3 vec);
	btVector3 glmToBt(vec3 vec);
	transform btToGlm(btTransform tr);

	quat btToGlm(btQuaternion ori);
	btQuaternion glmToBt(quat quat);
	btTransform glmToBt(transform tr);
}