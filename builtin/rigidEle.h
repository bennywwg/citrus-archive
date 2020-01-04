#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "bulletObject.h"

#include "world.h"
#include "../mankern/element.h"
#include "shapeEle.h"
#include "util.h"

namespace citrus {
	class rigidEle : public bulletObject, public element{
		float _mass;
		vec3 _inertiaMoment;
		bool _kinematic;
		float _friction;
		float _restitution;
		
		btMotionState *_motionState;

		worldShape *_shape;

		btRigidBody *_body;

		world *_world;

		transform _lastTr;

	public:
		// whether or not to allow overriding of body transform
		bool checkUpdates = true;

		void setVelocity(vec3 const& vel) const;
		vec3 getVelocity() const;

		void applyImpulse(vec3 const& force) const;

		void setPos(vec3 const& pos) const;
		vec3 getPos() const;

		void setOri(quat const& pos) const;
		quat getOri() const;

		void action();

		void deserialize(json const& j);
		json serialize();
		
		rigidEle(entRef const& ent, manager& man, void* usr);
		~rigidEle();
	};
}
