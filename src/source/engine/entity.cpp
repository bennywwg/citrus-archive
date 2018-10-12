#include <engine/entity.h>

namespace citrus {
	namespace engine {
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

			for(auto& e : _elements)
				if(e.type == type)
					return e.ele;

			return nullptr;
		}

		entity::entity(const std::vector<elementMeta>& toCreate, engine* eng, const std::string& name, const uint64_t id) : _elements(toCreate), name(name), id(id), eng(eng) { }

		uint64_t entityRef::id() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->id;
		}
		std::string entityRef::name() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->name;
		}
		engine* entityRef::eng() const {
			return _ptr->eng;
		}

		void entityRef::setLocalTransform(const transform &trans) {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			_ptr->setLocalTransform(trans);
		}
		void entityRef::setLocalPosition(const glm::vec3& pos) {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			_ptr->setLocalPosition(pos);
		}
		void entityRef::setLocalOrientation(const glm::quat& ori) {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			_ptr->setLocalOrientation(ori);
		}
		transform entityRef::getLocalTransform() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->getLocalTransform();
		}
		glm::vec3 entityRef::getLocalPosition() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->getLocalPosition();
		}
		glm::quat entityRef::getLocalOrientation() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->getLocalOrientation();
		}
		transform entityRef::getGlobalTransform() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->getGlobalTransform();
		}

		void entityRef::setParent(entityRef parent) {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			_ptr->setParent(parent._ptr);
		}
		entityRef entityRef::getRoot() {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return entityRef(_ptr->getRoot()->_this.lock());
		}
		entityRef entityRef::getParent() {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return entityRef(_ptr->getParent()->_this.lock());
		}
		std::vector<entityRef> entityRef::getChildren() {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			vector<entity*> rawChildren = _ptr->getChildren();
			vector<entityRef> res; res.reserve(rawChildren.size());
			for(entity* child : rawChildren) res.push_back(entityRef(child->_this));
			return res;
		}
		std::vector<entityRef> entityRef::getAllConnected() {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			vector<entity*> rawChildren = _ptr->getAllConnected();
			vector<entityRef> res; res.reserve(rawChildren.size());
			for(entity* child : rawChildren) res.push_back(entityRef(child->_this));
			return res;
		}

		bool entityRef::initialized() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->initialized();
		}
		bool entityRef::destroyed() const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->destroyed();
		}
		bool entityRef::valid() const {
			return !_ref.expired() && _ptr->valid();
		}

		element* entityRef::getElement(const std::type_index& type) const {
			if(!valid()) throw std::runtime_error("Invalid Entity");
			return _ptr->getElement(type);
		}

		entityRef entityRef::null() {
			return entityRef(std::shared_ptr<entity>(nullptr));
		}

		bool entityRef::operator==(const entityRef& other) const {
			return _ptr == other._ptr;
		}
		bool entityRef::operator==(entity* other) const {
			return _ptr == other;
		}
		bool entityRef::operator!=(const entityRef& other) const {
			return !(*this == other);
		}
		bool entityRef::operator!=(entity* other) const {
			return !(*this == other);
		}

		entityRef::entityRef(std::weak_ptr<entity> ref) : _ptr(ref.lock().get()), _ref(ref) { }
	}
}
