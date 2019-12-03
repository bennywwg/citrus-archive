#pragma once

#include <type_traits>
#include <string>

#include "element.h"

namespace citrus {
	class entRef;

	template<class T>
	class eleRef {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		friend class manager;
		friend class entRef;

		int64_t _id;
		T* _ptr;

	public:
		bool null() const;
		int64_t id() const;

		T& get() const;
		T& operator*() const;
		T* operator->() const;

		bool operator==(eleRef* other) const;
		bool operator!=(eleRef* other) const;
		operator bool() const;

		eleRef& operator=(const eleRef& other);

		eleRef();

	private:

		eleRef(T* ele);
	};
}
