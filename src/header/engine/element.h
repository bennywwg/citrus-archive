#pragma once

#ifndef ELEMENT_H
#define ELEMENT_H

#include <type_traits>
#include <typeindex>

#include <nlohmann/json.hpp>

#include <engine/entityRef.h>

namespace citrus::engine {
	class engine;
	class manager;

	class element {
		friend class manager;
		friend class entity;
		friend class entityRef;
		
		std::type_index _type;

	public:
		entityRef const ent;
		engine* const e;

		virtual void load(const nlohmann::json& parsed);
		virtual void preRender();
		virtual void render();
		virtual nlohmann::json save();

	protected:
		element(entityRef ent, std::type_index const& type);
	};
}

#endif