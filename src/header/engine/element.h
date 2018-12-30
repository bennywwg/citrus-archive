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
		bool _initialized = false;

		entityRef _ent;
		engine* _eng;

	public:
		entityRef ent() const;
		engine* eng() const;

		bool initialized() const;

		virtual void load(const nlohmann::json& parsed);
		virtual void preRender();
		virtual void render();
		virtual nlohmann::json save();

	protected:
		element(entityRef ent, std::type_index const& type);
	};
}

#endif