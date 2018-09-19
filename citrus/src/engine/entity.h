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

			bool _initialized = false;
			const std::vector<elementMeta> _elements;
			entity* _parent = nullptr;
			std::vector<entity*> _children;
			transform _trans;

			public:
			const std::string name;
			const uint64_t id;
			engine* const eng;

			void setLocalTransform(const transform &trans) {
				_trans = trans;
			}
			void setLocalPosition(const glm::vec3& pos) {
				_trans.setPosition(pos);
			}
			void setLocalOrientation(const glm::quat& ori) {
				_trans.setOrientation(ori);
			}
			transform getLocalTransform() const {
				return _trans;
			}
			glm::vec3 getLocalPosition() const {
				return _trans.getPosition();
			}
			glm::quat getLocalOrientation() const {
				return _trans.getOrientation();
			}
			transform getGlobalTransform() const {
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

			template<typename T> inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				return (T*)getElement(typeid(T));
			}
			inline element* getElement(const std::type_index& type) const {
				if(!_initialized) throw std::exception("You can't get an element before initialization");

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
	}
}

#endif