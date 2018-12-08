#include <engine/entity.h>
#include <engine/element.h>

namespace citrus::engine {
	using std::vector;
	using std::weak_ptr;

	void entity::setLocalTransform(const transform &trans) {
		_trans = trans;
	}
	void entity::setLocalPosition(const glm::vec3& pos) {
		_trans.setPosition(pos);
	}
	void entity::setLocalOrientation(const glm::quat& ori) {
		_trans.setOrientation(ori);
	}
	transform entity::getLocalTransform() const {
		return _trans;
	}
	glm::vec3 entity::getLocalPosition() const {
		return _trans.getPosition();
	}
	glm::quat entity::getLocalOrientation() const {
		return _trans.getOrientation();
	}
	transform entity::getGlobalTransform() const {
		if(_parent != nullptr) {
			return _trans * _parent->getGlobalTransform();
		} else {
			return _trans;
		}
	}

	void entity::setParent(entity* parent) {
		if(_parent != parent) {
			if(_parent == nullptr) {
				_parent = parent;
			} else {
				for(int i = 0; i < (int)_parent->_children.size(); ++i) {
					if(_parent->_children[i] == this) {
						_parent->_children.erase(_parent->_children.begin() + i); break;
					}
				}
				_parent = parent;
			}
			if(_parent != nullptr) {
				for(auto& child : _parent->getAllConnected()) {
					if(child == this) {
						_parent = nullptr;
						return;
					}
				}
				_parent->_children.push_back(this);
			}
		}
	}
	entity* entity::getRoot() {
		entity* res = this;
		while(res->_parent != nullptr) {
			res = res->_parent;
		}
		return res;
	}
	entity* entity::getParent() {
		return _parent;
	}
	std::vector<entity*> entity::getChildren() {
		return _children;
	}
	void entity::_accumulateAllChildren(std::vector<entity*>& accum) {
		for(auto& child : _children) {
			accum.push_back(child);
			child->_accumulateAllChildren(accum);
		}
	}
	std::vector<entity*> entity::getAllConnected() {
		std::vector<entity*> accum;
		auto root = getRoot();
		accum.push_back(root);
		root->_accumulateAllChildren(accum);
		return accum;
	}

	bool entity::initialized() const {
		return _initialized;
	}
	bool entity::destroyed() const {
		return _destroyed;
	}
	bool entity::valid() const {
		return _initialized && !_destroyed;
	}

	element* entity::getElement(const std::type_index& type) const {
		if(!_initialized) throw std::runtime_error("You can't get an element before initialization");

		for(element* e : _elements)
			if(e->_type == type)
				return e;

		return nullptr;
	}

	entity::entity(const std::vector<element*>& toCreate, engine* eng, const std::string& name, const uint64_t id) : _elements(toCreate), name(name), id(id), eng(eng) {
		this->_trans = transform();
	}
}
