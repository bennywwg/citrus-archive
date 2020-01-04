#include "bulletObject.h"

namespace citrus {
	void bulletObject::setFlags(int val) {
		_userData.flags = val;
	}
	int bulletObject::getFlags() const {
		return _userData.flags;
	}
	entRef bulletObject::getEnt() const {
		return entRef(_userData.ent);
	}
	userData::objectType bulletObject::getType() const {
		return _userData.type;
	}
	bulletObject::bulletObject(entity* ent, userData::objectType type, int flags) : _userData{ type, flags, ent } {
	}
}