#pragma once

#include "citrus/util.h"

namespace citrus {
	namespace dynamics {
		class collisionShape {
		private:
			std::unique_ptr<btCollisionShape> _shape;

		public:
			btCollisionShape* ptr() const;

			std::vector<std::pair<glm::vec3, glm::vec3>> getWireframe() const;

			collisionShape(float radius);
			collisionShape(float sx, float sy, float sz);
			collisionShape(vector<vec3> vertices);

			NO_COPY(collisionShape)
		};
	}
}
