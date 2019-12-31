#pragma once

#include <type_traits>
#include <typeindex>

#include "util.h"

#include <nlohmann/json.hpp>
#include "editorGrouping.h"

namespace citrus {
	class engine;
	class manager;
	class entity;
	class entRef;
	class ctEditor;

	using nlohmann::json;

	class element;

	struct elementInternalState {
		entity*					_ent;
		manager*				_man;
		void*					_usr;
		std::type_index			_type;
		element *prev, *next;
		bool destroyed;	
	};

	class element {
		friend class manager;
		friend class entity;
		friend class entRef;
		friend class ctEditor;
		template<class T> friend class eleRef;
	private:
		elementInternalState _state;
	public:

		elementInternalState _raw() const;

		manager& man() const;

		void *usr() const;

		// general utilty
		entRef		ent() const;

#ifdef CT_USE_EDITOR
		virtual grouping renderGUI();
#endif

		// load from json
		virtual void deserialize(citrus::json const & parsed);

		// save to json
		virtual citrus::json serialize();

		// load from architecture-specific blob
		// load from raw
		// return size of memory read from
		virtual uint64_t load(uint8_t * raw);


		// save to architecture-specific blob
		// save in raw
		// return size of memory saved
		virtual uint64_t save(uint8_t* raw);

		// do something
		virtual void action();

	protected:
		element(entRef const&, manager &, void *, std::type_index const&);
	};
}