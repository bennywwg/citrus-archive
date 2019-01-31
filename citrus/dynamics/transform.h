#pragma once

#include <citrus/util.h>

namespace citrus {
	class transform {
		vec3 _position;
		quat _orientation;
		vec3 _scale;

		public:
		vec3 getPosition() const;
		void setPosition(const vec3& position);

		quat getOrientation() const;
		void setOrientation(const quat& orientation);

		vec3 getScale() const;
		void setScale(const vec3& scale);

		transform getTranslated(vec3 trans);

		mat4 getMat() const;

		transform operator*(const transform& rhs) const;
		bool operator==(const transform& other) const;
		bool operator!=(const transform& other) const;

		transform();
		transform(const mat4& mat);
		transform(const vec3& position, const quat& orientation);
	};
}
