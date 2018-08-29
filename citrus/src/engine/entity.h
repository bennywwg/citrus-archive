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
			elementMeta(const std::type_index type, element* ele) : type(type), ele(ele) { }
		};

		class entity {
			friend class manager;

			bool _initialized = false;
			const std::vector<elementMeta> _elements;

			public:
			const std::string name;
			const uint64_t uuid;
			engine* const eng;

			entity* parent = nullptr;
			transform trans;

			bool initialized() const {
				return _initialized;
			}

			template<typename T> inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				return (T*)getElement(std::type_info(typeid(T)));
			}
			inline element* getElement(const std::type_index& type) const {
				if(!_initialized) throw std::exception("You can't get an element before it's initialized");

				for(auto& e : _elements)
					if(e.type == type)
						return e.ele;

				return nullptr;
			}

			private:
			entity(const std::vector<elementMeta>& toCreate, engine* eng, const std::string& name, const uint64_t uuid) :
				_elements(toCreate), eng(eng), name(name), uuid(uuid) { }
		};
	}
}

#endif