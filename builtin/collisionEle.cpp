#include "collisionEle.h"
#include "../mankern/entityRef.h"

namespace citrus {
	void collisionEle::action() {
		_object->setWorldTransform(glmToBt(ent().getGlobalTrans()));
	}
	bool collisionEle::active() {
		return std::find(_world->bodies.begin(), _world->bodies.end(), _object) != _world->bodies.end();
	}
	void collisionEle::activate() {
		_world->addObject(_object);
	}
	void collisionEle::deactivate() {
		_world->removeObject(_object);
	}
	collisionEle::collisionEle(entRef const& ent, manager& man, void* usr) :
		bulletObject(ent._raw(), userData::collisionObject),
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
		_object->setUserPointer(&data);
		data.type = userData::collisionObject;
		data.ent = ent._raw();

		_world->addObject(_object);

		_object->setWorldTransform(glmToBt(ent.getGlobalTrans()));
	}
	collisionEle::~collisionEle() {
		_world->removeObject(_object);
		delete _object;
		delete _shape;
	}
}