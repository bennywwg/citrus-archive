#include "collisionEle.h"
#include "../mankern/entityRef.h"

namespace citrus {
	void collisionEle::action() {
		_object->setWorldTransform(glmToBt(ent().getGlobalTrans()));
	}
	collisionEle::collisionEle(entRef const& ent, manager& man, void* usr) :
		element(ent, man, usr, typeid(collisionEle)),
		_friction(1.0f), _restitution(0.1f),
		_world((world*)usr)
	{
		eleRef<shapeEle> const& myShape = ent.getEle<shapeEle>();
		if (!myShape.id()) throw std::runtime_error("collisionEle needs shapeEle");

		// make shape in world
		_shape = new worldShape(myShape->getShape());

		// make body
		_object = new btCollisionObject();
		_object->setCollisionShape(_shape->wdshape);
		_object->setUserPointer((void*)"collisionEle");
		_object->setFriction(_friction);
		_object->setRestitution(_restitution);

		_world->addObject(_object);

		_object->setWorldTransform(glmToBt(ent.getGlobalTrans()));
	}
	collisionEle::~collisionEle() {
		_world->removeObject(_object);
		delete _object;
		delete _shape;
	}
}