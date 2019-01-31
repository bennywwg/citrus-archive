#include <graphics/camera/camera.h>

#include <util/glmUtil.h>

namespace citrus {
	namespace engine {
		void camera::setGimbalAngles(float angleX, float angleY) {
			trans.setOrientation(toQuat(
				rotate(angleY, vec3(0.0f, 1.0f, 0.0f)) *
				rotate(angleX, vec3(1.0f, 0.0f, 0.0f))
			));
		}
		geom::line camera::getRayFromScreenSpace(vec2 ss) {
			const vec4 toUntransformNear = vec4(ss.x, ss.y, 0.0f, 1.0f);
			const vec4 toUntransformFar = vec4(ss.x, ss.y, 1.0f, 1.0f);
			const mat4 invViewProjection = inverse(getViewProjectionMatrix());
			glm::vec4 nearw = invViewProjection * toUntransformNear;
			glm::vec4 farw = invViewProjection * toUntransformFar;
			return geom::line(
				vec3(nearw) / nearw.w,
				vec3(farw) / farw.w
			);
		}
		mat4 camera::getViewMatrix() {
			return inverse(translate(trans.getPosition()) * toMat4(trans.getOrientation()));
		}
		mat4 camera::getProjectionMatrix() {
			return glm::perspective(verticalFOV, aspectRatio, zNear, zFar);
		}
		mat4 camera::getViewProjectionMatrix() {
			return getProjectionMatrix() * getViewMatrix();
		}
	}
}
