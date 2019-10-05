#include "citrus/graphics/camera.h"
#include "citrus/util.h"

namespace citrus::graphics {
	void camera::setGimbalAngles(float angleX, float angleY) {
		ori = glm::toQuat(
			glm::rotate(angleY, vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(angleX, vec3(1.0f, 0.0f, 0.0f))
		);
	}
	geom::line camera::getRayFromScreenSpace(vec2 ss) const {
		const vec4 toUntransformNear = vec4(ss.x, ss.y, 0.0f, 1.0f);
		const vec4 toUntransformFar = vec4(ss.x, ss.y, 1.0f, 1.0f);
		const mat4 invViewProjection = inverse(getViewProjectionMatrix());
		glm::vec4 nearw = invViewProjection * toUntransformNear;
		glm::vec4 farw = invViewProjection * toUntransformFar;
		return {
			vec3(nearw) / nearw.w,
			vec3(farw) / farw.w
		};
	}
	mat4 camera::getViewMatrix() const {
		return glm::inverse(glm::translate(pos) * glm::toMat4(ori));
	}
	mat4 camera::getProjectionMatrix() const {
		return glm::perspective(verticalFOV, aspectRatio, zNear, zFar);
	}
	mat4 camera::getViewProjectionMatrix() const {
		return getProjectionMatrix()* getViewMatrix();
	}

	vec2 camera::worldToScreen(vec3 const& ws) const {
		vec4 tr = getViewProjectionMatrix() * vec4(ws.x, ws.y, ws.z, 1.0f);
		return vec3(tr) / tr.w;
	}

	frustrumCullInfo camera::genFrustrumInfo() const {
		frustrumCullInfo res;

		float vertAngle = glm::radians(verticalFOV) * 0.5f;
		float horiAngle = glm::atan(glm::tan(vertAngle) * aspectRatio);

		//construct plane vectors in camera coordinate space 
		res.pzdir = vec3(0.0f, 0.0f, 1.0f);
		res.nzdir = vec3(0.0f, 0.0f, -1.0f);
		res.tpdir = vec3(0.0f, glm::cos(vertAngle), glm::sin(vertAngle));
		res.bpdir = vec3(0.0f, -res.tpdir.y, res.tpdir.z);
		res.rpdir = vec3(glm::cos(horiAngle), 0.0f, glm::sin(horiAngle));
		res.lpdir = vec3(-res.rpdir.x, 0.0f, res.rpdir.z);


		//convert to world space
		mat3 camMat = glm::toMat3(ori);
		res.pzdir = camMat * res.pzdir;
		res.nzdir = camMat * res.nzdir;
		res.tpdir = camMat * res.tpdir;
		res.bpdir = camMat * res.bpdir;
		res.rpdir = camMat * res.rpdir;
		res.lpdir = camMat * res.lpdir;

		//calculate limits using camera position;
		res.pzlim = glm::dot(pos, res.pzdir) - zNear;
		res.nzlim = glm::dot(pos, res.nzdir) + zFar;
		res.tplim = glm::dot(pos, res.tpdir);
		res.bplim = glm::dot(pos, res.bpdir);
		res.rplim = glm::dot(pos, res.rpdir);
		res.lplim = glm::dot(pos, res.lpdir);

		return res;
	}

	bool frustrumCullInfo::testSphere(vec3 pos, float rad) const {
		bool r0 = (glm::dot(pos, pzdir) - rad < pzlim);
		bool r1 = (glm::dot(pos, nzdir) - rad < nzlim);
		bool r2 = (glm::dot(pos, rpdir) - rad < rplim);
		bool r3 = (glm::dot(pos, tpdir) - rad < tplim);
		bool r4 = (glm::dot(pos, lpdir) - rad < lplim);
		bool r5 = (glm::dot(pos, bpdir) - rad < bplim);
		if (!(r0 && r1 && r2 && r3 && r4 && r5)) {
			//system("");
		}
		return r0 && r1&& r2&& r3&& r4&& r5;
	}
}