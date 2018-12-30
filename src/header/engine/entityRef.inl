#include <engine/entityRef.h>

namespace citrus::engine {
	template<class T>
	eleRef<T> entityRef::getElement() {
		static_assert(std::is_base_of<element, T>::value, "can only get element if the type is derived from class element");
		if(null()) return eleRef<T>();
		return eleRef<T>(*this, _ref.lock()->getElement<T>());
	}
}