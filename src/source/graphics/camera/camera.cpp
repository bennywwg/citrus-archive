#include <graphics/camera/camera.h>

#include <util/util.h>
#include <glm/ext.hpp>

namespace citrus {
	namespace engine {
		void camera::setGimbalAngles(float angleX, float angleY) {
			trans.setOrientation(glm::toQuat(
				glm::rotate(angleY, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(angleX, glm::vec3(1.0f, 0.0f, 0.0f))
			));
		}
		geom::line camera::getRayFromScreenSpace(vec2 ss) {
			const glm::vec4 toUntransformNear = glm::vec4(ss.x, ss.y, -1.0f, 1.0f);
			const glm::vec4 toUntransformFar = glm::vec4(ss.x, ss.y, 1.0f, 1.0f);
			const glm::mat4 invViewProjection = inverse(getViewProjectionMatrix());
			return geom::line(
				glm::vec3(invViewProjection * toUntransformNear),
				glm::vec3(invViewProjection * toUntransformFar)
			);
		}
		glm::mat4 camera::getViewMatrix() {
			return glm::translate(-trans.getPosition()) * glm::inverse(glm::toMat4(trans.getOrientation()));
		}
		glm::mat4 camera::getProjectionMatrix() {
			return glm::perspective(verticalFOV, aspectRatio, zNear, zFar);
		}
		glm::mat4 camera::getViewProjectionMatrix() {
			return getProjectionMatrix() * getViewMatrix();
		}
	}
}
