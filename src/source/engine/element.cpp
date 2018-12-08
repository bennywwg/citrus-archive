#include <engine/element.h>
#include <engine/entity.h>
#include <engine/entityRef.h>

namespace citrus {
	namespace engine {
		void element::load(const nlohmann::json & parsed) { }
		void element::preRender() { }
		void element::render() { }
		nlohmann::json element::save() { return nlohmann::json::object(); }

		element::element(entityRef ent, std::type_index const & type) : _type(type), ent(ent), e(ent.eng()) { }
	}
}