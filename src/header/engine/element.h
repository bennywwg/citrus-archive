#pragma once

#ifndef ELEMENT_H
#define ELEMENT_H

#include <nlohmann/json.hpp>

#include "entity.h"

namespace citrus {
	namespace engine {
		class engine;
		class manager;

		class element {
			friend class manager;
		
			std::type_index _type;

			public:
			entityRef const ent;
			engine* const e;

			inline virtual void load(const nlohmann::json& parsed) { }
			inline virtual void preRender() { }
			inline virtual void render() { }
			inline virtual nlohmann::json save() { return nlohmann::json::object(); }

		//private:
			inline element(entityRef ent, std::type_index const& type) : _type(type), ent(ent), e(ent.eng()) { }
		};
	}
}

#endif // ! ELEMENT_H
