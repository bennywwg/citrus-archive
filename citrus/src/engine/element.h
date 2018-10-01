#pragma once

#ifndef  ELEMENT_H
#define ELEMENT_H

#include <nlohmann/json.hpp>

#include "entity.h"

namespace citrus {
	namespace engine {
		class engine;

		class element {
			public:
			entityRef const ent;
			engine* const e;

			inline virtual void load(const nlohmann::json& parsed) { }
			inline virtual void preRender() { }
			inline virtual void render() { }
			inline virtual nlohmann::json save() { return nlohmann::json::object(); }

			inline element(entityRef ent) : ent(ent), e(ent.g) { }
		};
	}
}

#endif // ! ELEMENT_H
