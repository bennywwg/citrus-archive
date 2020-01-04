#pragma once

#include "../mankern/entity.h"
#include "../mankern/entityRef.h"

namespace citrus {
	struct userData {
		enum objectType {
			collisionObject,
			sensorObject,
			rigidBody,
			otherType
		};
		objectType type;
		int flags;
		entity* ent;
	};

	class bulletObject {
	protected:
		userData _userData;
	public:
		void setFlags(int val);
		int getFlags() const;
		entRef getEnt() const;
		userData::objectType getType() const;

		bulletObject(entity* ent, userData::objectType type, int flags = ~int(0));
	};
}