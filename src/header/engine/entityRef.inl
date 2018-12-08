#include <engine/entityRef.h>

namespace citrus::engine {
	template<class T>
	eleRef<T> entityRef::getElement() {
		static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
		return eleRef<T>(_ptr->getElement<T>());
	}
}