#pragma once

#include <type_traits>
#include <string>

namespace citrus::engine {
	class entityRef;

	class eleDereferenceException {
		public:
		std::string er;
		inline eleDereferenceException(const std::string& er) : er(er) {
		}
	};

	template<class T>
	class eleRef {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		friend class entity;
		friend class entityRef;
		friend class manager;
		
		entityRef _owner;
		T* _ptr;

		public:
		bool null() const;
		bool valid() const;

		T& get() const;
		T& operator*() const;
		T* operator->() const;

		bool operator==(const eleRef& other) const;
		bool operator==(std::nullptr_t other) const;
		operator bool() const;

		eleRef& operator=(const eleRef& other);
		eleRef& operator=(std::nullptr_t other);

		eleRef();
		eleRef(T* ele);
	};
}