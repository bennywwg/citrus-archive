#pragma once

#include "citrus/util.h"
#include "citrus/dynamics/geom.h"

namespace citrus::graphics {
	struct frustrumCullInfo {
		//ncp, fcp, right plane, top plane, left plane, bottom plane, respectively
		vec3 pzdir, nzdir, rpdir, tpdir, lpdir, bpdir;

		//respective thresholds for each dir
		float pzlim, nzlim, rplim, tplim, lplim, bplim;

		//returns true if sphere needs to be rendered
		bool testSphere(vec3 pos, float rad) const;
	};

	class camera {
	public:
		vec3 pos = vec3(0.0f, 0.0f, 0.0f);
		quat ori = quat(1.0f, 0.0f, 0.0f, 0.0f);

		float verticalFOV = 90.0f;
		float aspectRatio = 1.0f;
		float zNear = 0.01f;
		float zFar = 100.0f;

		void setGimbalAngles(float angleX, float angleY);

		geom::line getRayFromScreenSpace(glm::vec2 ss = glm::vec2(0, 0)) const;

		mat4 getViewMatrix() const;
		mat4 getProjectionMatrix() const;
		mat4 getViewProjectionMatrix() const;

		frustrumCullInfo genFrustrumInfo() const;
	};
}