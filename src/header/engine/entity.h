#pragma once

#ifndef ENTITY_H
#define ENTIYT_H

#include <vector>
#include <typeindex>
#include <memory>

#include <dynamics/transform.h>

namespace citrus {
	namespace engine {
		class element;
		class engine;

		class elementMeta {
			public:
			std::type_index type;
			element* const ele;
			inline elementMeta(const std::type_index type, element* ele) : type(type), ele(ele) { }
		};

		//this class is extremely tightly coupled to manager, modify with caution
		class entity {
			friend class manager;
			friend class entityRef;

			bool _initialized = false;
			bool _destroyed = false;
			const std::vector<elementMeta> _elements;
			entity* _parent = nullptr;
			std::vector<entity*> _children;
			transform _trans;
			std::weak_ptr<entity> _this;

			public:
			const std::string name;
			const uint64_t id;
			engine* const eng;

			inline void setLocalTransform(const transform &trans) {
				_trans = trans;
			}
			inline void setLocalPosition(const glm::vec3& pos) {
				_trans.setPosition(pos);
			}
			inline void setLocalOrientation(const glm::quat& ori) {
				_trans.setOrientation(ori);
			}
			inline transform getLocalTransform() const {
				return _trans;
			}
			inline glm::vec3 getLocalPosition() const {
				return _trans.getPosition();
			}
			inline glm::quat getLocalOrientation() const {
				return _trans.getOrientation();
			}
			inline transform getGlobalTransform() const {
				if(_parent != nullptr) {
					return _trans * _parent->getGlobalTransform();
				} else {
					return _trans;
				}
			}

			inline void setParent(entity* parent) {
				if(_parent != parent) {
					if(_parent == nullptr) {
						_parent = parent;
					} else {
						for(int i = 0; i < _parent->_children.size(); ++i) {
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
			inline entity* getRoot() {
				entity* res = this;
				while(res->_parent != nullptr) {
					res = res->_parent;
				}
				return res;
			}
			inline entity* getParent() {
				return _parent;
			}
			inline std::vector<entity*> getChildren() {
				return _children;
			}
			private:
			inline void _accumulateAllChildren(std::vector<entity*>& accum) {
				for(auto& child : _children) {
					accum.push_back(child);
					child->_accumulateAllChildren(accum);
				}
			}
			public:
			inline std::vector<entity*> getAllConnected() {
				std::vector<entity*> accum;
				auto root = getRoot();
				accum.push_back(root);
				root->_accumulateAllChildren(accum);
				return accum;
			}

			inline bool initialized() const {
				return _initialized;
			}
			inline bool destroyed() const {
				return _destroyed;
			}
			inline bool valid() const {
				return _initialized && !_destroyed;
			}

			template<typename T> inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				return (T*)getElement(typeid(T));
			}
			inline element* getElement(const std::type_index& type) const {
				if(!_initialized) throw std::runtime_error("You can't get an element before initialization");

				for(auto& e : _elements)
					if(e.type == type)
						return e.ele;

				return nullptr;
			}

			public:
			constexpr static uint64_t nullID = std::numeric_limits<uint64_t>::max();

			private:
			entity(const std::vector<elementMeta>& toCreate, engine* eng, const std::string& name, const uint64_t id) :
				_elements(toCreate), eng(eng), name(name), id(id) { }
		};

		class entityRef {
			friend class manager;

			entity* _ptr;
			std::shared_ptr<entity> _ref;

			public:

			uint64_t id() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->id;
			}
			std::string name() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->name;
			}
			engine* eng() const {
				return _ptr->eng;
			}

			inline void setLocalTransform(const transform &trans) {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				_ptr->setLocalTransform(trans);
			}
			inline void setLocalPosition(const glm::vec3& pos) {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				_ptr->setLocalPosition(pos);
			}
			inline void setLocalOrientation(const glm::quat& ori) {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				_ptr->setLocalOrientation(ori);
			}
			inline transform getLocalTransform() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->getLocalTransform();
			}
			inline glm::vec3 getLocalPosition() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->getLocalPosition();
			}
			inline glm::quat getLocalOrientation() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->getLocalOrientation();
			}
			inline transform getGlobalTransform() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->getGlobalTransform();
			}

			inline void setParent(entityRef parent) {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				_ptr->setParent(parent._ptr);
			}
			inline entityRef getRoot() {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return entityRef(_ptr->getRoot()->_this.lock());
			}
			inline entityRef getParent() {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return entityRef(_ptr->getParent()->_this.lock());
			}
			inline std::vector<entityRef> getChildren() {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				auto rawChildren = _ptr->getChildren();
				std::vector<entityRef> res; res.reserve(rawChildren.size());
				for(auto& child : rawChildren) res.push_back(child->_this.lock());
				return res;
			}
			inline std::vector<entityRef> getAllConnected() {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				auto rawChildren = _ptr->getAllConnected();
				std::vector<entityRef> res; res.reserve(rawChildren.size());
				for(auto& child : rawChildren) res.push_back(child->_this.lock());
				return res;
			}

			inline bool initialized() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->initialized();
			}
			inline bool destroyed() const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->destroyed();
			}
			inline bool valid() const {
				return _ptr && _ptr->valid();
			}

			template<typename T> inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return (T*)_ptr->getElement(typeid(T));
			}
			inline element* getElement(const std::type_index& type) const {
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return _ptr->getElement(type);
			}

			/*inline entityRef(const entityRef& other) : entityRef(other._ref) { }
			inline entityRef(entityRef&& other) : _ptr(other._ptr), _ref(std::move(other._ref)) { }
			inline entityRef& operator=(const entityRef& other) {
				_ptr = other._ptr;
				_ref = other._ref;
			}*/

			inline static entityRef null() {
				return entityRef(std::shared_ptr<entity>(nullptr));
			}

			inline bool operator==(const entityRef& other) const {
				return _ptr == other._ptr;
			}
			inline bool operator==(entity* other) const {
				return _ptr == other;
			}
			inline bool operator!=(const entityRef& other) const {
				return !(*this == other);
			}
			inline bool operator!=(entity* other) const {
				return !(*this == other);
			}

			constexpr static uint64_t nullID = std::numeric_limits<uint64_t>::max();

			private:

			entityRef(std::shared_ptr<entity> ref) : _ref(ref), _ptr(ref.get()) { }
		};
	}
}

#endif
