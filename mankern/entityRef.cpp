#include "entityRef.h"
#include "entity.h"

#include "exceptions.h"
#include "util.h"

namespace citrus {
#ifdef CITRUS_EDITOR
	using std::vector;
	std::unique_ptr<editor::gui> entRef::renderGUI() const {
		editor::container* res = new editor::container();
		res->title = "Entity: " + name() + " (" + std::to_string(id()) + ")";

		for (int i = 0; i < this->_ref.lock()->_elements.size(); i++)
			res->items.emplace_back(this->_ref.lock()->_elements[i].second->renderGUI());

		return std::unique_ptr<editor::gui>((editor::gui*)res);
	}
#endif // CITRUS_EDITOR

	entity* entRef::_raw() const {
		return _ptr;
	}

	bool entRef::null() const {
		return !(_id && _id == _ptr->id);
	}
	int64_t entRef::id() const {
		return null() ? 0 : _id;
	}

	void entRef::setName(std::string const& st) const {
		if (null()) throw nullEntityException("setName: null entity");
		_ptr->name = st;
	}

	std::string entRef::name() const {
		if (null()) throw nullEntityException("name: null entity");
		return _ptr->name;
	}

	entRef entRef::getParent() const {
		if (null()) throw nullEntityException("getParent: null entity");
		return entRef(_ptr->parent);
	}
	std::vector<entRef> entRef::getChildren() const {
		if (null()) throw nullEntityException("getParent: null entity");
		std::vector<entRef> res;
		res.reserve(_ptr->children.size());
		for (size_t i = 0; i < _ptr->children.size(); i++) {
			res.push_back(entRef(_ptr->children[i]));
		}
		return res;
	}

	// local spatial function

	void entRef::setLocalPos(vec3 const& pos) const {
		if (null()) throw nullEntityException("setLocalPos: null entity");
		_ptr->trans.setPosition(pos);
	}
	void entRef::setLocalOri(quat const& ori) const {
		if (null()) throw nullEntityException("setLocalOri: null entity");
		_ptr->trans.setOrientation(ori);
	}
	void entRef::setLocalTrans(transform const& trans) const {
		if (null()) throw nullEntityException("setLocalTrans: null entity");
		_ptr->trans = trans;
	}
	vec3 entRef::getLocalPos() const {
		if (null()) throw nullEntityException("getLocalPos: null entity");
		return _ptr->trans.getPosition();
	}
	quat entRef::getLocalOri() const {
		if (null()) throw nullEntityException("getLocalOri: null entity");
		return _ptr->trans.getOrientation();
	}
	transform entRef::getLocalTrans() const {
		if (null()) throw nullEntityException("getLocalTrans: null entity");
		return _ptr->trans;
	}

	transform entRef::getGlobalTrans() const {
		if (null()) throw nullEntityException("getGlobalTrans: null entity");
		if (!_ptr->parent) return _ptr->trans;
		transform res = _ptr->trans;
		entity* cur = _ptr->parent;
		while (cur) {
			res = res * cur->trans;
			cur = cur->parent;
		}
		return res;
	}

	bool entRef::operator==(entRef const& other) const {
		return other ? id() == other.id() : null();
	}
	bool entRef::operator!=(entRef const& other) const {
		return !(*this == other);
	}
	entRef::operator bool() const {
		return !null();
	}

	entRef& entRef::operator=(const entRef& other) {
		_ptr = other._ptr;
		_id = other._id;
		return *this;
	}

	entRef::entRef() : entRef(nullptr) { }
	entRef::entRef(entity* e) {
		if (e && e->id) {
			_id = abs(e->id);
			_ptr = e;
		} else {
			_id = 0;
			_ptr = nullptr;
		}
	}
}