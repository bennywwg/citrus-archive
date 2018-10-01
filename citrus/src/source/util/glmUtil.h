#pragma once

#ifndef GLMUTIL_H
#define GLMUTIL_H

#include <glm/ext.hpp>
#include <btBulletDynamicsCommon.h>

using glm::quat;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::uvec2;
using glm::uvec3;
using glm::uvec4;
using glm::mat4;

namespace citrus {
	namespace util {
		vec3 btToGlm(btVector3 vec);
		btVector3 glmToBt(vec3 vec);
		quat btToGlm(btQuaternion quat);
		btQuaternion glmToBt(quat quat);
	}
}

#endif