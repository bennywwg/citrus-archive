#pragma once

#ifndef SPACE_H
#define SPACE_H

#include "glmUtil.h"

namespace citrus {
	namespace engine {
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

			transform();
			transform(vec3 position, quat orientation);
		};
	}
}

#endif