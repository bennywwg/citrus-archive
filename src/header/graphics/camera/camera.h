#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <util/util.h>
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

			geom::line getRayFromScreenSpace(vec2 ss = vec2(0, 0));

			mat4 getViewMatrix();
			mat4 getProjectionMatrix();
			mat4 getViewProjectionMatrix();
		};
	}
}

#endif