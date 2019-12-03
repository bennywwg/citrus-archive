#include "element.h"
#include "entityRef.h"

namespace citrus {
#ifdef CITRUS_EDITOR
	std::unique_ptr<editor::gui> element::renderGUI() {
		editor::container* res = new editor::container();
		res->title = "Implement the renderGUI() function for editing";
		return std::unique_ptr<editor::gui>(res);
	}
#endif

	manager& element::man() const {
		return *_man;
	}
	void *element::usr() const {
		return _usr;
	}
	entRef element::ent() const {
		return entRef(_ent);
	}

	void element::deserialize(citrus::json const& parsed) { }
	citrus::json element::serialize() { return { }; }
	uint64_t element::load(uint8_t* data) { return 0; }
	uint64_t element::save(uint8_t* data) { return 0; }

	void element::action() { }

	element::element(entRef const& ent, manager &man, void *usr, std::type_index const & type) :
		_ent(ent._ptr),
		_man(&man),
		_usr(usr),
		_type(type),
		destroyed(false) { }
}