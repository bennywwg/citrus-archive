#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "world.h"
#include "../mankern/element.h"
#include "shapeEle.h"
#include "util.h"

namespace citrus {
	class rigidEle : public element {
		float _mass;
		float _friction;
		float _restitution;
		
		btMotionState *_state;

		worldShape *_shape;

		btRigidBody *_body;

		world *_world;

		transform _lastTr;
	public:
		// whether or not to allow overriding of body transform
		bool checkUpdates = true;



		inline void setVelocity(vec3 const& vel) const {
			_body->setLinearVelocity(glmToBt(vel));
		}
		inline vec3 const& getVelocity() const {
			return btToGlm(_body->getLinearVelocity());
		}

		inline void action() {
			if (checkUpdates && _lastTr != ent().getLocalTrans()) {
				_body->setWorldTransform(glmToBt(ent().getLocalTrans()));
				_lastTr = ent().getLocalTrans();
			} else {
				_lastTr = btToGlm(_body->getWorldTransform());
				ent().setLocalTrans(_lastTr);
			}
		}

		inline rigidEle(entRef const& ent, manager & man, void* usr) :
			element(ent, man, usr, typeid(rigidEle)),
			_mass(1.0f), _friction(1.0f), _restitution(0.1f),
			_state(new btDefaultMotionState()),
			_world((world*)usr)
		{
			eleRef<shapeEle> const& myShape = ent.getEle<shapeEle>();
			if (!myShape.id()) throw std::runtime_error("rigidEle needs shapeEle");

			// make shape in world
			_shape = new worldShape(myShape->getShape());

			// construction info
			btRigidBody::btRigidBodyConstructionInfo ci = btRigidBody::btRigidBodyConstructionInfo(_mass, _state, _shape->wdshape);
			ci.m_friction = _friction;
			ci.m_restitution = _restitution;

			// make body
			_body = new btRigidBody(ci);
			_body->setUserPointer(this);
			_body->setActivationState(DISABLE_DEACTIVATION);

			_world->addBody(_body);
		}
		inline ~rigidEle() {
			_world->removeBody(_body);
			delete _body;
			delete _shape;
			delete _state;
		}
	};
}
