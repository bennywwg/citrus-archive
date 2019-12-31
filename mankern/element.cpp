#include "element.h"
#include "entityRef.h"

namespace citrus {
#ifdef CT_USE_EDITOR
	grouping element::renderGUI() {
		return { };
	}
#endif

	elementInternalState element::_raw() const {
		return _state;
	}

	manager& element::man() const {
		return *_state._man;
	}
	void *element::usr() const {
		return _state._usr;
	}
	entRef element::ent() const {
		return entRef(_state._ent);
	}

	void element::deserialize(citrus::json const& parsed) { }
	citrus::json element::serialize() { return { }; }
	uint64_t element::load(uint8_t* data) { return 0; }
	uint64_t element::save(uint8_t* data) { return 0; }

	void element::action() { }

	element::element(entRef const& ent, manager &man, void *usr, std::type_index const & type) :
		_state{ent._ptr, &man, usr, type, nullptr, nullptr, false} { }
}