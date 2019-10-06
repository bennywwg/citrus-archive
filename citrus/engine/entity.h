#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <typeindex>
#include <memory>

#include "citrus/dynamics/transform.h"
#include "citrus/engine/element.h"

namespace citrus {
	namespace engine {
		class engine;

		class entity {
			friend class manager;
			friend class entityRef;

			const std::vector<std::pair<std::type_index, element*>> _elements;
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
			void setLocalScale(const glm::vec3& scale);
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

			template<class T>
			inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				return (T*)getElement(typeid(T));
			}
			element* getElement(const std::type_index& type) const;
			vector<element*> getElements() const;

			public:
			constexpr static uint64_t nullID = 0;// std::numeric_limits<uint64_t>::max();

			private:
			entity(const std::vector<std::pair<std::type_index, element*>>& toCreate, engine* eng, const std::string& name, const uint64_t id);

		};
	}
}

#endif
