#include <engine/entityRef.h>
#include <engine/entity.h>
#include <mutex>

namespace citrus::engine {
	using std::vector;

	bool entityRef::null() const {
		return _ref.expired() || _ref.lock()->destroyed();
	}

	bool entityRef::valid() const {
		return !null() & _ref.lock()->initialized();
	}
	bool entityRef::initialized() const {
		return !_ref.expired() && _ref.lock()->initialized();
	}
	bool entityRef::destroyed() const {
		return !_ref.expired() && _ref.lock()->destroyed();
	}

	uint64_t entityRef::id() const {
		return null() ? nullID : _ref.lock()->id;
	}
	std::string entityRef::name() const {
		if(null()) throw std::runtime_error("name(): null entity");
		return _ref.lock()->name;
	}
	engine* entityRef::eng() const {
		return _eng;
	}

	void entityRef::setLocalTransform(const transform &trans) const {
		if(null()) throw entDereferenceException("setLocalTransform(): null entity");
		_ref.lock()->setLocalTransform(trans);
	}
	void entityRef::setLocalPosition(const glm::vec3& pos) const {
		if(null()) throw entDereferenceException("setLocalPosition(): null entity");
		_ref.lock()->setLocalPosition(pos);
	}
	void entityRef::setLocalOrientation(const glm::quat& ori) const {
		if(null()) throw entDereferenceException("setLocalOrientation(): null entity");
		_ref.lock()->setLocalOrientation(ori);
	}
	transform entityRef::getLocalTransform() const {
		if(null()) throw entDereferenceException("getLocalTransform(): null entity");
		return _ref.lock()->getLocalTransform();
	}
	glm::vec3 entityRef::getLocalPosition() const {
		if(null()) throw entDereferenceException("getLocalPosition(): null entity");
		return _ref.lock()->getLocalPosition();
	}
	glm::quat entityRef::getLocalOrientation() const {
		if(null()) throw entDereferenceException("getLocalOrientation(): null entity");
		return _ref.lock()->getLocalOrientation();
	}
	transform entityRef::getGlobalTransform() const {
		if(null()) throw entDereferenceException("getGlobalTransform(): null entity");
		return _ref.lock()->getGlobalTransform();
	}

	void entityRef::setParent(entityRef parent) const {
		if(null()) throw entDereferenceException("setParent(): null entity");
		_ref.lock()->setParent(parent._ref.lock());
	}
	entityRef entityRef::getRoot() const {
		if(null()) throw entDereferenceException("getRoot(): null entity");
		return entityRef(_ref.lock()->getRoot()->_this.lock());
	}
	entityRef entityRef::getParent() const {
		if(null()) throw entDereferenceException("getParent(): null entity");
		return entityRef(_ref.lock()->getParent()->_this.lock());
	}
	std::vector<entityRef> entityRef::getChildren() const {
		if(null()) throw entDereferenceException("getChildren(): null entity");
		vector<entity*> rawChildren = _ref.lock()->getChildren();
		vector<entityRef> res; res.reserve(rawChildren.size());
		for(entity* child : rawChildren) res.push_back(entityRef(child->_this));
		return res;
	}
	std::vector<entityRef> entityRef::getAllConnected() const {
		if(null()) throw entDereferenceException("getAllConnected(): null entity");
		vector<entity*> rawChildren = _ref.lock()->getAllConnected();
		vector<entityRef> res; res.reserve(rawChildren.size());
		for(entity* child : rawChildren) res.push_back(entityRef(child->_this));
		return res;
	}

	bool entityRef::operator==(const entityRef& other) const {
		return (_ref == other._ref);
	}
	bool entityRef::operator!=(const entityRef& other) const {
		return !(*this == other);
	}
	entityRef::entityRef() : entityRef(std::weak_ptr<entity>()) { }
	entityRef::entityRef(std::weak_ptr<entity> ref) : _ptr(ref.lock().get()), _ref(ref) { }
}