#pragma once

#ifndef COLLISIONSHAPE_H
#define COLLISIONSHAPE_H

#include "stdUtil.h"
#include "glmUtil.h"

namespace citrus {
	namespace dynamics {
		class collisionShape {
		private:
			shared_ptr<btCollisionShape> _shape;

		public:
		weak_ptr<btCollisionShape> ptr() const;

			explicit collisionShape(vector<vec3> vertices);

			NO_COPY(collisionShape)
		};
	}
}

#endif