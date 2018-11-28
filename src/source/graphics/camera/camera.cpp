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
			const vec4 toUntransformNear = vec4(ss.x, ss.y, -1.0f, 1.0f);
			const vec4 toUntransformFar = vec4(ss.x, ss.y, 1.0f, 1.0f);
			const mat4 invViewProjection = inverse(getViewProjectionMatrix());
			return geom::line(
				vec3(invViewProjection * toUntransformNear),
				vec3(invViewProjection * toUntransformFar)
			);
		}
		mat4 camera::getViewMatrix() {
			return translate(-trans.getPosition()) * inverse(toMat4(trans.getOrientation()));
		}
		mat4 camera::getProjectionMatrix() {
			return glm::perspective(verticalFOV, aspectRatio, zNear, zFar);
		}
		mat4 camera::getViewProjectionMatrix() {
			return getProjectionMatrix() * getViewMatrix();
		}
	}
}
