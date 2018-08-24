#pragma once

#ifndef ENTITY_H
#define ENTIYT_H

#include <vector>
#include <typeindex>

#include <dynamics/transform.h>

namespace citrus {
	namespace engine {
		class element;
		class engine;

		struct elementMeta {
			const int _size;
			const std::type_index _type;
			element* const _ele;
			elementMeta(const int& size, const std::type_index& type, element* ele) :
				_size(size), _type(type), _ele(ele) { }
		};

		class entity {
			friend class manager;

			bool _initialized = false;
			const std::vector<elementMeta> _elements;

			public:
			engine* const eng;
			transform trans;

			bool initialized() const {
				return _initialized;
			}

			template<typename T> inline T* getElement() const {
				static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
				return (T*)getElement(std::type_info(typeid(T)));
			}
			inline element* getElement(const std::type_index& type) const {
				if(!_initialized) throw std::exception("You can't get an entity component before it's initialized");

				for(auto& e : _elements)
					if(e._type == type)
						return e._ele;

				return nullptr;
			}

			private:
			entity(const std::vector<elementMeta>& toCreate, engine* eng) : _elements(toCreate), eng(eng) { }
		};
	}
}

#endif