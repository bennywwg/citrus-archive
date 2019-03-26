#include "citrus/dynamics/collisionShape.h"
#include "citrus/util.h"

namespace citrus {
	namespace dynamics {
		btCollisionShape* collisionShape::ptr() const {
			return _shape.get();
		}
		std::vector<std::pair<glm::vec3, glm::vec3>> collisionShape::getWireframe() const {
			int type = _shape->getShapeType();
			if(type == SPHERE_SHAPE_PROXYTYPE) {
				std::vector<std::pair<glm::vec3, glm::vec3>> res;
				for(int i = 0; i < 8; i++) {
					float sint1 = glm::sin(i * 2.0f * 3.1415926f / 8.0f);
					float cost1 = glm::cos(i * 2.0f * 3.1415926f / 8.0f);
					float sint2 = glm::sin((i + 1) * 2.0f * 3.1415926f / 8.0f);
					float cost2 = glm::cos((i + 1) * 2.0f * 3.1415926f / 8.0f);

					res.emplace_back(glm::vec3(cost1, 0.0f, sint1), glm::vec3(cost2, 0.0f, sint2));
					res.emplace_back(glm::vec3(cost1, sint1, 0.0f), glm::vec3(cost2, sint2, 0.0f));
					res.emplace_back(glm::vec3(0.0f, cost1, sint1), glm::vec3(0.0f, cost2, sint2));
				}
				return res;
			} else if(type == BOX_SHAPE_PROXYTYPE) {
				btBoxShape* box = reinterpret_cast<btBoxShape*>(_shape.get());
				float hx = box->getHalfExtentsWithMargin().getX();
				float hy = box->getHalfExtentsWithMargin().getY();
				float hz = box->getHalfExtentsWithMargin().getZ();
				return std::vector<std::pair<glm::vec3, glm::vec3>>{
					{glm::vec3(-hx, hy, -hz), glm::vec3(-hx, hy, hz)},
					{glm::vec3(-hx, hy, hz), glm::vec3(hx, hy, hz)},
					{glm::vec3(hx, hy, hz), glm::vec3(hx, hy, -hz)},
					{glm::vec3(hx, hy, -hz), glm::vec3(-hx, hy, -hz)},

					{glm::vec3(-hx, -hy, -hz), glm::vec3(-hx, -hy, hz)},
					{glm::vec3(-hx, -hy, hz), glm::vec3(hx, -hy, hz)},
					{glm::vec3(hx, -hy, hz), glm::vec3(hx, -hy, -hz)},
					{glm::vec3(hx, -hy, -hz), glm::vec3(-hx, -hy, -hz)},

					{glm::vec3(-hx, hy, -hz), glm::vec3(-hx, -hy, -hz)},
					{glm::vec3(hx, hy, -hz), glm::vec3(hx, -hy, -hz)},
					{glm::vec3(-hx, hy, hz), glm::vec3(-hx, -hy, hz)},
					{glm::vec3(hx, hy, hz), glm::vec3(hx, -hy, hz)},
				};
			} else {
				return std::vector<std::pair<glm::vec3, glm::vec3>>{
				};
			}
		}
		collisionShape::collisionShape(float radius) :
			_shape(new btSphereShape(radius)) {
		}
		collisionShape::collisionShape(float halfX, float halfY, float halfZ) :
			_shape(new btBoxShape(btVector3(halfX, halfY, halfZ))) {
		}
		collisionShape::collisionShape(vector<vec3> vertices) :
			_shape(new btConvexHullShape((btScalar*)util::glmToBtVector(vertices).data(), int(vertices.size()))) { }
	}
}