#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/ext.hpp>
#include <dynamics/transform.h>
#include <dynamics/geometry/geom.h>

namespace citrus {
	namespace engine {
		class camera {
		public:
			transform trans;

			float verticalFOV = 90.0f;
			float aspectRatio = 1.0f;
			float zNear = 0.001f;
			float zFar = 100.0f;

			void setGimbalAngles(float angleX, float angleY);

			geom::line getRayFromScreenSpace(glm::vec2 ss = glm::vec2(0, 0));

			glm::mat4 getViewMatrix();
			glm::mat4 getProjectionMatrix();
			glm::mat4 getViewProjectionMatrix();
		};
	}
}

#endif
