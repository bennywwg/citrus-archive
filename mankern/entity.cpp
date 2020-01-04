#include "entity.h"

namespace citrus {
	transform entity::globalTrans() {
		if (!parent) return trans;
		mat4 res = trans.getMat();
		entity* cur = parent;
		while (cur) {
			res = cur->trans.getMat() * res;
			cur = cur->parent;
		}	
		return res;
	}
	entity::entity(std::string const& name, int64_t id) : name(name), id(id) { }
	entity::~entity() {
		id = 0;
	}
}
