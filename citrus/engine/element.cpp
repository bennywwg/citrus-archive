#include "citrus/engine/element.h"
#include "citrus/engine/entity.h"
#include "citrus/engine/entityRef.h"

namespace citrus {
	namespace engine {
		entityRef element::ent() const {
			return _ent;
		}
		engine* element::eng() const {
			return _eng;
		}
		bool element::initialized() const {
			return _initialized;
		}

		std::unique_ptr<editor::gui> element::renderGUI() {
			editor::container* res = new editor::container();
			res->title = "Implement the renderGUI() function for editing";
			return std::unique_ptr<editor::gui>(res);
		}

		void element::load(const nlohmann::json & parsed) { }
		void element::preRender() { }
		void element::render() { }
		nlohmann::json element::save() { return nlohmann::json::object(); }

		element::element(entityRef ent, std::type_index const & type) : _type(type), _ent(ent), _eng(ent.eng()) { }
	}
}