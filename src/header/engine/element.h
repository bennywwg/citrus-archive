#pragma once

#ifndef ELEMENT_H
#define ELEMENT_H

#include <type_traits>
#include <typeindex>

#include <nlohmann/json.hpp>

namespace citrus {
	namespace engine {
		class engine;
		class manager;

		class element {
			friend class manager;
			friend class entity;
			friend class entityRef;
		
			std::type_index _type;

		public:
			entityRef const ent;
			engine* const e;

			inline virtual void load(const nlohmann::json& parsed);
			inline virtual void preRender();
			inline virtual void render();
			inline virtual nlohmann::json save();

		private:
			inline element(entityRef ent, std::type_index const& type);
		};

		template<class T>
		class eleRef {
			static_assert(std::is_base_of<element, T>::value, "T must be derived from element");

			entityRef _owner;
			T* _ptr;

			public:
			bool initialied() const {
				return _owner.initialized();
			}
			bool valid() const {
				return _owner.valid();
			}
			bool destroyed() const {
				return _owner.destroyed();
			}

			T& get() const {
				if(!valid()) throw std::runtime_error("eleRef is not valid");
				return *_ptr;
			}
			T& operator*() const {
				return get();
			}

			eleRef() : _owner(entityRef::null()), _ptr(nullptr) {
			}
			eleRef(entityRef ref) : _owner(ref), _ptr(ref.getElement<T>()) {
			}
			eleRef(T* ele) : _owner(ele->ent), _ptr(ele) {
			}
		};
	}
}

#endif