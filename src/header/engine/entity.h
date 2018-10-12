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

			void setLocalTransform(const transform &trans);
			void setLocalPosition(const glm::vec3& pos);
			void setLocalOrientation(const glm::quat& ori);
			transform getLocalTransform() const;
			glm::vec3 getLocalPosition() const;
			glm::quat getLocalOrientation() const;
			transform getGlobalTransform() const;

			void setParent(entity* parent);
			entity* getRoot();
			entity* getParent();
			std::vector<entity*> getChildren();
			private:
			void _accumulateAllChildren(std::vector<entity*>& accum);
			public:
			std::vector<entity*> getAllConnected();

			bool initialized() const;
			bool destroyed() const;
			bool valid() const;

			template<class T>
			inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				return (T*)getElement(typeid(T));
			}
			element* getElement(const std::type_index& type) const;

			public:
			constexpr static uint64_t nullID = std::numeric_limits<uint64_t>::max();

			private:
			entity(const std::vector<elementMeta>& toCreate, engine* eng, const std::string& name, const uint64_t id);

		};

		class entityRef {
			friend class manager;

			entity* _ptr;
			std::weak_ptr<entity> _ref;

			public:

			uint64_t id() const;
			std::string name() const;
			engine* eng() const;

			void setLocalTransform(const transform &trans);
			void setLocalPosition(const glm::vec3& pos);
			void setLocalOrientation(const glm::quat& ori);
			transform getLocalTransform() const;
			glm::vec3 getLocalPosition() const;
			glm::quat getLocalOrientation() const;
			transform getGlobalTransform() const;

			void setParent(entityRef parent);
			entityRef getRoot();
			entityRef getParent();
			std::vector<entityRef> getChildren();
			std::vector<entityRef> getAllConnected();

			bool initialized() const;
			bool destroyed() const;
			bool valid() const;

			template<typename T>
			inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				if(!valid()) throw std::runtime_error("Invalid Entity");
				return (T*)_ptr->getElement(typeid(T));
			}
			element* getElement(const std::type_index& type) const;

			/*entityRef(const entityRef& other) : entityRef(other._ref) { }
			entityRef(entityRef&& other) : _ptr(other._ptr), _ref(std::move(other._ref)) { }
			entityRef& operator=(const entityRef& other) {
				_ptr = other._ptr;
				_ref = other._ref;
			}*/

			static entityRef null();

			bool operator==(const entityRef& other) const;
			bool operator==(entity* other) const;
			bool operator!=(const entityRef& other) const;
			bool operator!=(entity* other) const;

			constexpr static uint64_t nullID = std::numeric_limits<uint64_t>::max();

			private:

			entityRef(std::weak_ptr<entity> ref);
		};
	}
}

#endif
