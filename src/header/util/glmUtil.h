#pragma once

#ifndef GLMUTIL_H
#define GLMUTIL_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
//#include <btBulletDynamicsCommon.h>

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
using glm::mat3;
using glm::mat4;

using glm::translate;
using glm::inverse;
using glm::toMat4;
using glm::toQuat;

//namespace citrus {
//	namespace util {
//		vec3 btToGlm(btVector3 vec);
//		btVector3 glmToBt(vec3 vec);
//		quat btToGlm(btQuaternion quat);
//		btQuaternion glmToBt(quat quat);
//	}
//}

#endif