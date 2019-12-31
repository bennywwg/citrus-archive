#include "rigidEle.h"
#include "../mankern/entityRef.h"

namespace citrus {
	void rigidEle::setVelocity(vec3 const& vel) const {
		_body->setLinearVelocity(glmToBt(vel));
	}
	vec3 rigidEle::getVelocity() const {
		return btToGlm(_body->getLinearVelocity());
	}
	void rigidEle::applyImpulse(vec3 const& force) const {
		_body->applyCentralImpulse(glmToBt(force));
	}
	void rigidEle::setPos(vec3 const& pos) const {
		auto tr = _body->getWorldTransform();
		tr.setOrigin(glmToBt(pos));
		_body->setWorldTransform(tr);
	}
	vec3 rigidEle::getPos() const {
		return btToGlm(_body->getWorldTransform().getOrigin());
	}
	void rigidEle::setOri(quat const& ori) const {
		auto tr = _body->getWorldTransform();
		tr.setRotation(glmToBt(ori));
		_body->setWorldTransform(tr);
	}
	quat rigidEle::getOri() const {
		return btToGlm(_body->getWorldTransform().getRotation());
	}
	void rigidEle::action() {
		ent().setLocalTrans(btToGlm(_body->getWorldTransform()));
		//if (checkUpdates && _lastTr != ent().getLocalTrans()) {
		//	_body->setWorldTransform(glmToBt(ent().getLocalTrans()));
		//	_lastTr = ent().getLocalTrans();
		//} else {
		//	_lastTr = btToGlm(_body->getWorldTransform());
		//	ent().setLocalTrans(_lastTr);
		//}
	}
	void rigidEle::deserialize(json const& j) {
		_mass = j["mass"].get<float>();
		_inertiaMoment = loadVec3(j["inertiaMoment"]);
		_body->setMassProps(_mass, glmToBt(_inertiaMoment));
		_friction = j["friction"].get<float>();
		_body->setFriction(_friction);
		if (j["kinematic"].get<bool>()) _body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CollisionFlags::CF_KINEMATIC_OBJECT);
	}
	json rigidEle::serialize() {
		return {
			{ "kinematic", _kinematic },
			{ "mass", _mass },
			{ "friction", _friction },
			{ "inertiaMoment", citrus::save(_inertiaMoment) }
		};
	}
	rigidEle::rigidEle(entRef const& ent, manager& man, void* usr) :
		element(ent, man, usr, typeid(rigidEle)),
		_mass(1.0f), _friction(1.0f), _restitution(0.1f),
		_motionState(new btDefaultMotionState()),
		_world((world*)usr)
	{
		eleRef<shapeEle> const& myShape = ent.getEle<shapeEle>();
		if (!myShape.id()) throw std::runtime_error("rigidEle needs shapeEle");

		// make shape in world
		_shape = new worldShape(myShape->getShape());

		// construction info
		btRigidBody::btRigidBodyConstructionInfo ci = btRigidBody::btRigidBodyConstructionInfo(_mass, _motionState, _shape->wdshape, btVector3(1.0, 1.0, 1.0));
		ci.m_friction = _friction;
		ci.m_restitution = _restitution;

		// make body
		_body = new btRigidBody(ci);
		_body->setUserPointer((void*)"rigidEle");
		_body->setActivationState(DISABLE_DEACTIVATION);

		_world->addBody(_body);

		setPos(ent.getLocalPos());
		setOri(ent.getLocalOri());
	}
	rigidEle::~rigidEle() {
		_world->removeBody(_body);
		delete _body;
		delete _shape;
		delete _motionState;
	}
}