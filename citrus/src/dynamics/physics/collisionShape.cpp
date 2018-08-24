#include <dynamics/physics/collisionShape.h>

#include <util/stdUtil.h>
#include <util/glmUtil.h>
#include <util/util.h>

namespace citrus {
	namespace dynamics {
		weak_ptr<btCollisionShape> collisionShape::ptr() const {
			return _shape;
		}
		collisionShape::collisionShape(vector<vec3> vertices) :
			_shape(new btConvexHullShape((btScalar*)util::glmToBtVector(vertices).data(), int(vertices.size()))) { }
	}
}

