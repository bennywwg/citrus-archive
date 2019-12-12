#pragma once

#include <vector>
#include <string>

#include "util.h"

namespace citrus {
	class element;
	class ctEditor;

	class entity {
	public:
		transform trans;


		transform globalTrans();
		
		// =  0 -> unallocated
		// >  0 -> valid
		int64_t id = 0;

		entity *prev = nullptr, *next = nullptr;
		entity* parent = nullptr;
		std::vector<entity*> children;

		std::vector<element*> eles;
		
		std::string name;

		bool destroyed = false;

		entity(std::string const& name, int64_t id);
		~entity();
	};
}