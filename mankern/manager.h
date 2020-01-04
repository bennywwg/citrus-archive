#pragma once

#include <map>
#include <typeinfo>
#include <vector>
#include <typeindex>
#include <functional>

#include <iostream>


#include <nlohmann/json.hpp>


#include "entity.h"
#include "entityRef.h"
#include "elementRef.h"
#include "element.h"
#include "exceptions.h"

namespace citrus {
	using std::map;
	using std::vector;
	using std::function;
	using std::tuple;
	using std::pair;
	using std::string;
	using std::type_info;
	using std::type_index;
	
	using nlohmann::json;

	class manager;

	struct eleInitPayload {
		element *which;
		optional_t<json> data;
		optional_t<std::vector<uint8_t>> binData;
		std::function<void(element*)> init; //inherently optional
		eleInitPayload(element* which) : which(which) {
		}
		eleInitPayload(element* which, json const& j) : which(which), data(j) {
		}
		eleInitPayload(element* which, std::vector<uint8_t> const& v) : which(which), binData(v) {
		}
		eleInitPayload(element* which, std::function<void(element*)> const& i) : which(which), init(i) {
		}
	};

	class ctEditor;

	class manager {
		friend class entity;
		friend class ctEditor;

		constexpr static size_t maxEnts = 512;

		int _frame;
		double _dt;
		bool _stopped;

		entity*	entities;
		entity *allocBegin, *allocEnd, *freeBegin;
		int64_t	currentID;

		// initialize entity data
		void einit();

		// release memory
		void ecleanup();

		// create entity
		entity* ealloc(std::string const& name);

		// delete entity
		void efree(entity* ptr);
		
		// remove the parent of child if it has a parent
		void eclearRelation(entity* child);

		// both pointers must be valid
		void esetRelation(entity* parent, entity* child);

		struct elementInfo {
			string name;

			manager* parentManager;
			element *data = nullptr;
			element *allocBegin = nullptr, *allocEnd = nullptr, *freeBegin = nullptr;
			std::vector<eleInitPayload> toCreate, toCreateSwap;
			std::vector<element*> toDestroy, toDestroySwap;

			type_index type;

			bool active;
			void* usrPtr;
			int size;

			function<void(element*, entity*)> ctor;
			function<void(element*)> dtor;
			
			element* access(size_t index);

			void flushToDestroy();

			void flushToCreate();

			void action();

			void renderGUI(vector<grouping> &groups);

			// initialize this element data
			void elinit();

			// release memory
			void elcleanup();

			// take slot
			element* elalloc();

			// free slot
			void elfree(element* ptr);

			elementInfo(type_index const& ty, manager* man) : type(ty), parentManager(man) { }
		};
	
		//manager state
		struct typeElePair {
			type_index type;
			elementInfo* info;
			typeElePair(type_index const& ty, elementInfo* inf) : type(ty), info(inf) { }
		};
		vector<typeElePair> _userData;
		vector<entity*> _toDestroy, _entities;

	public:
		elementInfo* getInfo(type_index const& index);
		elementInfo* getInfoByName(string const& name);

		json remapEleInitIDs(json remappedData, std::map<int64_t, entRef>const& remappedIDs);

		//template<class T> eleRef<T> dereferenceElement(const json& data);
		//template<class T> json referenceElement(entRef ent);
		bool isEntityReference(const json& data);
		bool isElementReference(const json& data);

		void destroyElement(element* ele);

		element* addElement(entRef ent, elementInfo* ei);
		element* addElement(entRef ent, elementInfo* ei, std::vector<uint8_t> const& binData);
		element* addElement(entRef ent, elementInfo* ei, json const& j);

		void allChildren(entity* ent, vector<entRef>& out);


public:
		int									  frame();
		double								   time();
		void								   stop();
		bool								stopped();

		void						  flushToCreate();
		void						 flushToDestroy();
		void								 action();
		void							  renderGUI(vector<grouping> &groups);

		template<class T>
		std::vector<eleRef<T>>				 ofType();

		std::vector<entRef>					allEnts();

		template<class T> void		   registerType(string name, bool active, void* usrPtr);
		entRef								 create(string const& name);
		template<typename T> eleRef<T>	 addElement(entRef ent);
		template<typename T> eleRef<T>	 addElement(entRef ent, std::vector<uint8_t> const& binData);
		template<typename T> eleRef<T>	 addElement(entRef ent, json const& j);
		template<typename T> void	 destroyElement(eleRef<T> ele);
		void								destroy(entRef ent);
		void							setRelation(entRef parent, entRef child);
		void						  clearRelation(entRef child);

		json						serializePrefab(entRef const& toSave);
		entRef					  deserializePrefab(json const& data);

		manager();
		~manager();

private:
		manager(const manager& other) = delete;
		manager& operator=(const manager& other) = delete;
	};
}