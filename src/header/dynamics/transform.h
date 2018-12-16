#pragma once

#include <util/glmUtil.h>

namespace citrus {
	class transform {
		vec3 _position;
		quat _orientation;

		public:
		vec3 getPosition() const;
		void setPosition(vec3 position);

		quat getOrientation() const;
		void setOrientation(quat orientation);

		transform getTranslated(vec3 trans);

		mat4 getMat() const;

		transform operator*(const transform& rhs) const;
		bool operator==(const transform& other) const;
		bool operator!=(const transform& other) const;

		transform();
		transform(mat4 mat);
		transform(vec3 position, quat orientation);
	};
}