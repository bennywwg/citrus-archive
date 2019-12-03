#include "entity.h"

namespace citrus {
	transform entity::globalTrans() {
		entity* par = parent;
		transform res = trans;
		while (par) {
			res = par->trans * trans;
			par = par->parent;
		}
		return res;
	}
	entity::entity(std::string const& name, int64_t id) : name(name), id(id) { }
	entity::~entity() {
		id = 0;
	}
}
