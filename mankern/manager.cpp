#include "manager.h"
#include "elementRef.inl"
#include "util.h"

namespace citrus {
	element* manager::elementInfo::access(size_t index) {
		return (element*)(((uint8_t*)data) + index * size);
	}

	void manager::elementInfo::flushToDestroy() {
		// remove element pointers from all used lists
		for (int i = 0; i < toDestroySwap.size(); i++) {
			auto& eleList = toDestroySwap[i]->_state._ent->eles;
			for (int j = 0; j < eleList.size(); j++) {
				if (eleList[j] == toDestroySwap[i]) {
					eleList.erase(eleList.begin() + j);
					break;
				}
			}
			// NUANCE zone:
			// dtor(toDestroy[i]) may "take" the memory toDestroy[i] (by calling elalloc(...))
			// points to, but the ctor(toDestroy[i]) will not be called
			// until after this function returns, and the only members
			// used will be next and prev, which are never changed
			// in the ctor or dtor. So this should work
			elfree(toDestroySwap[i]);
		}

		for (int i = 0; i < toDestroySwap.size(); i++) {
			// make sure to preserve next and prev through dtor call
			// (standard probably not guaranteed to preserve these)
			element* oldNext = toDestroySwap[i]->_state.next, * oldPrev = toDestroySwap[i]->_state.prev;
			dtor(toDestroySwap[i]);
			memset(toDestroySwap[i], '\0', size);
			toDestroySwap[i]->_state._type = type;
			toDestroySwap[i]->_state.next = oldNext;
			toDestroySwap[i]->_state.prev = oldPrev;
		}
	}

	void manager::elementInfo::flushToCreate() {
		for (int i = 0; i < toCreateSwap.size(); i++) {
			// again, preserve next and prev through ctor
			element* oldNext = toCreateSwap[i].which->_state.next, * oldPrev = toCreateSwap[i].which->_state.prev;
			ctor(toCreateSwap[i].which, toCreateSwap[i].which->_state._ent);
			if (toCreateSwap[i].binData.has_value()) toCreateSwap[i].which->load(toCreateSwap[i].binData.value().data());
			if (toCreateSwap[i].data.has_value()) toCreateSwap[i].which->deserialize(toCreateSwap[i].data.value());
			toCreateSwap[i].which->_state.next = oldNext;
			toCreateSwap[i].which->_state.prev = oldPrev;
		}
		/*for (int i = 0; i < toCreateSwap.size(); i++) {
			toCreateSwap[i].which->_state._ent->eles.push_back(toCreateSwap[i].which);
		}*/
	}

	void manager::elementInfo::action() {
		if (active) {
			for (element* el = allocBegin; el; el = el->_state.next) {
				if(el->_state._man) el->action();
			}
		}
	}

	void manager::elementInfo::renderGUI(vector<grouping> &groups) {
		for (element* el = allocBegin; el; el = el->_state.next) {
			if (el->_state._man) {
				auto gp = el->renderGUI();
				if (!gp.data.empty()) groups.push_back(gp);
			}
		}
	}

	// initialize this element data
	void manager::elementInfo::elinit() {
		data = (element*)calloc(maxEnts, size);
		freeBegin = data;
		access(maxEnts - 1)->_state.next = nullptr;
		for (int64_t i = maxEnts - 2; i >= 0; i--) {
			access(i)->_state.next = access(i + 1);
		}
	}

	// release memory
	void manager::elementInfo::elcleanup() {
		free(data);
	}

	// take slot
	element* manager::elementInfo::elalloc() {
		element* res = freeBegin;
		if (res) {
			freeBegin = freeBegin->_state.next;

			if (!allocBegin) allocBegin = res;
			res->_state.next = nullptr;
			res->_state.prev = allocEnd;
			if (allocEnd) allocEnd->_state.next = res;
			allocEnd = res;
		}
		return res;
	}

	// free slot
	void manager::elementInfo::elfree(element* ptr) {
		if (ptr->_state.prev)
			ptr->_state.prev->_state.next = ptr->_state.next;
		else
			allocBegin = ptr->_state.next;

		if (ptr->_state.next)
			ptr->_state.next->_state.prev = ptr->_state.prev;
		else
			allocEnd = ptr->_state.prev;

		ptr->_state.next = freeBegin;
		ptr->_state.prev = nullptr; // shouldn't ever be used
		freeBegin = ptr;
	}

	// both pointers must be valid
	void manager::esetRelation(entity* parent, entity* child) {
		if (child->parent == parent || parent == child) return;
		for (entity* cur = parent->parent; cur; cur = cur->parent) {
			if (cur == child) return; // no cycles
		}
		eclearRelation(child);
		child->parent = parent;
		parent->children.push_back(child);
	}

	// initialize entity data
	void manager::einit() {
		currentID = 1;
		entities = (entity*)calloc(maxEnts, sizeof(entity));
		entities[maxEnts - 1].next = nullptr;
		for (int i = maxEnts - 2; i >= 0; i--) {
			entities[i].next = &entities[i + 1];
		}
		freeBegin = entities;
		allocBegin = nullptr;
		allocEnd = nullptr;
	}

	// release memory
	void manager::ecleanup() {
		free(entities);
	}

	// create entity
	entity* manager::ealloc(std::string const& name) {
		entity* res = freeBegin;
		if (res) {
			freeBegin = res->next;

			new (res) entity(name, currentID);

			if (!allocBegin) allocBegin = res;
			res->next = nullptr;
			res->prev = allocEnd;
			if (allocEnd) allocEnd->next = res;
			allocEnd = res;
			currentID++;
		}
		return res;
	}

	// delete entity
	void manager::efree(entity* ptr) {
		eclearRelation(ptr);
		for (int i = ptr->children.size() - 1; i >= 0; i--) eclearRelation(ptr->children[i]);

		if (ptr->prev)
			ptr->prev->next = ptr->next;
		else
			allocBegin = ptr->next;

		if (ptr->next)
			ptr->next->prev = ptr->prev;
		else
			allocEnd = ptr->prev;

		ptr->next = freeBegin;
		ptr->prev = nullptr; // shouldn't ever be used
		freeBegin = ptr;

		if (ptr->id) ptr->entity::~entity();
		ptr->id = 0;
	}

	// remove the parent of child if it has a parent
	void manager::eclearRelation(entity* child) {
		if (child->parent)
			for (int i = 0; i < child->parent->children.size(); i++)
				if(child->parent->children[i] == child)
					child->parent->children.erase(child->parent->children.begin() + i);
		child->parent = nullptr;
	}

	manager::elementInfo* manager::getInfo(type_index const& index) {
		for (auto const& kvp : _userData) if (kvp.type == index) return kvp.info;
		return nullptr;
	}

	manager::elementInfo* manager::getInfoByName(string const& name) {
		for (auto const& kvp : _userData) if (kvp.info->name == name) return kvp.info;
		return nullptr;
	}

	json manager::remapEleInitIDs(json remappedData, std::map<int64_t, entRef> const& remappedIDs) {
		recursive_iterate(remappedData, [this, &remappedIDs](json::iterator it) {
			if (this->isEntityReference(*it) || this->isElementReference(*it)) {
				auto found = remappedIDs.find((*it)["ID"].get<int64_t>());
				if (found != remappedIDs.end()) {
					(*it)["ID"] = (*found).second.id();
				} else {
					throw invalidPrefabHierarchyException("did not find mapped ID");
				}
			}
			});
		return remappedData;
	}

	bool manager::isEntityReference(const json& data) {
		auto foundType = data.find("Type");
		if (foundType == data.end() || !foundType.value().is_string() || foundType.value().get<string>() != "Entity Reference") return false;
		auto foundID = data.find("ID");
		if (foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
		return true;
	}

	bool manager::isElementReference(const json& data) {
		auto foundType = data.find("Type");
		if (foundType == data.end() || !foundType.value().is_string() || foundType.value().get<string>() != "Element Reference") return false;
		auto foundName = data.find("Name");
		if (foundName == data.end() || !foundName.value().is_string() || getInfoByName(foundName.value().get<string>()) == nullptr) return false;
		auto foundID = data.find("ID");
		if (foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
		return true;
	}

	std::vector<entRef> manager::allEnts() {
		std::vector<entRef> res;
		for (entity* ent = allocBegin; ent; ent = ent->next) {
			res.push_back(entRef(ent));
		}
		return res;
	}

	void manager::destroyElement(element* ele) {
		if (!ele) throw nullEntityException("destroyElement() : invalid element");
		elementInfo* inf = getInfo(ele->_state._type);
		if (ele->_state.destroyed) return;
		ele->_state.destroyed = true;
		inf->toDestroy.push_back(ele);
	}

	element* manager::addElement(entRef ent, manager::elementInfo *inf) {
		if (!ent) throw nullEntityException("addElement() : invalid entity");
		auto const& t = inf->type;
		for (auto const& e : ent._ptr->eles) if (e->_state._type == t)
#ifndef IGNORE_DUPLICATE_OP
			throw std::runtime_error("duplicate entity error");
#else
			return;
#endif
		element* res = inf->elalloc();
		res->_state._type = t;
		res->_state._ent = ent._ptr;
		inf->toCreate.emplace_back(res);
		ent._ptr->eles.push_back(res);
		return res;
	}

	element* manager::addElement(entRef ent, manager::elementInfo* inf, vector<uint8_t> const& binData) {
		element* res = addElement(ent, inf);
		inf->toCreate.back().binData = binData;
		return res;
	}

	element* manager::addElement(entRef ent, manager::elementInfo* inf, json const& j) {
		element* res = addElement(ent, inf);
		inf->toCreate.back().data = j;
		return res;
	}

	void manager::allChildren(entity* ent, vector<entRef>& out) {
		for (size_t i = 0; i < ent->children.size(); i++) {
			out.push_back(entRef(ent->children[i]));
		}
		for (size_t i = 0; i < ent->children.size(); i++) {
			allChildren(ent->children[i], out);
		}
	}

	entRef manager::create(std::string const& name) {
		return ealloc(name);
	}

	void manager::destroy(entRef ent) {
		if (ent.id()) {
			if (ent._ptr->destroyed) {
				return;
			}
			ent._ptr->destroyed = true;
			_toDestroy.push_back(ent._ptr);
			// TODO: perhaps linearize this?
			for (int i = 0; i < ent._ptr->children.size(); i++) {
				destroy(ent._ptr->children[i]);
			}
		}
		/*else {
			throw nullEntityException("destroy null entity");
		}*/
	}

	void manager::setRelation(entRef parent, entRef child) {
		if (parent && child) {
			esetRelation(parent._ptr, child._ptr);
		}
	}

	void manager::clearRelation(entRef child) {
		if (child) eclearRelation(child._ptr);
	}

	int manager::frame() {
		return _frame;
	}

	double manager::time() {
		return _frame * _dt;
	}

	void manager::stop() {
		_stopped = true;
	}

	bool manager::stopped() {
		return _stopped;
	}

	void manager::flushToCreate() {
		for (auto& kvp : _userData) {
			kvp.info->toCreateSwap = kvp.info->toCreate;
			kvp.info->toCreate.clear();
		}
		for (auto kvp : _userData) {
			kvp.info->flushToCreate();
		}
	}

	void manager::flushToDestroy() {
		// destroy all elements of the entities to destroy
		for (auto e : _toDestroy) {
			for (auto el : e->eles) {
				if (!el->_state.destroyed) {
					destroyElement(el);
				}
			}
		}

		// copy destroy buffer of all to swap destroy buffer
		// then clear destroy buffer
		for (auto& kvp : _userData) {
			kvp.info->toDestroySwap = kvp.info->toDestroy;
			kvp.info->toDestroy.clear();
		}

		// copy destroy buffer
		auto list = _toDestroy;
		_toDestroy.clear();

		// destroy all queued elements
		for (auto& kvp : _userData) {
			kvp.info->flushToDestroy();
		}

		// destroy the actual entities
		for (auto e : list) {
			efree(e);
		}
	}

	void manager::action() {
		for (auto& kvp : _userData) {
			kvp.info->action();
		}
	}

	void manager::renderGUI(vector<grouping> &groups) {
		for (auto& kvp : _userData) {
			kvp.info->renderGUI(groups);
		}
	}

	json manager::serializePrefab(entRef const& toSave) {
		if (!toSave) throw nullEntityException("Cannot save null entity");
		vector<entRef> connected = { toSave };
		allChildren(toSave._ptr, connected);
		json::array_t entList = json::array();
		for (entRef ent : connected) {
			json::array_t entElements = json::array();
			for (element* ele : ent._ptr->eles) {
				entElements.push_back({
					{ "Name", getInfo(ele->_state._type)->name },
					{ "Init", ele->serialize() }
				});
			}

			json j = {
				{ "Name", ent.name() },
				{ "ID", ent.id() },
				{ "Parent", ent.getParent().id() },
				{ "Transform", save(ent.getLocalTrans()) },
				{ "Elements", entElements }
			};
			entList.push_back(j);
		}
		return json{
			{"Entities", entList }
		};
	}

	entRef manager::deserializeTree(json const& data) {
		bool isTree = true;

		json::array_t remappedEntities;
		std::map<int64_t, entRef> idMap;
		entRef res;

		{
			for (json const& entDesc : data["Entities"]) {
				entRef er;
				if (entDesc.find("Load") == entDesc.end()) {
					er = ealloc(entDesc["Name"]);
				} else {
					string content = loadEntireFile(entDesc["Load"]);
					json j = json::parse(content);
					er = deserializeTree(j);
				}
				int64_t id = entDesc["ID"].get<int64_t>();
				if (idMap.find(id) != idMap.end()) throw invalidPrefabHierarchyException("deserializePrefab: duplicate entity id");
				idMap[id] = er;
				if (!res) res = er; // first element is always the base node in the hierarchy
			}

			try {
				for (json const& entDesc : data["Entities"]) {
					remappedEntities.push_back(remapEleInitIDs(entDesc, idMap));
				}
			} catch (invalidPrefabHierarchyException const& ex) {
				for (auto const& kvp : idMap) {
					efree(kvp.second._ptr);
				}
				throw ex;
			}
		}

		{
			for (json const& entDesc : remappedEntities) {
				string name = entDesc["Name"].get<string>();
				transform trans = loadTransform(entDesc["Transform"]);
				int64_t parentID = entDesc["Parent"].get<int64_t>();
				json::array_t const& elementsJson = entDesc["Elements"];

				entRef ent = idMap[entDesc["ID"].get<int64_t>()];
				if (parentID && idMap.find(parentID) == idMap.end()) throw invalidPrefabHierarchyException("deserializePrefab: parent entity not found");
				entRef parent = parentID ? idMap[parentID] : entRef();

				if (parent) setRelation(parent, ent);
				ent.setLocalTrans(trans);

				for (json const& eleDesc : elementsJson) {
					elementInfo* inf = getInfoByName(eleDesc["Name"].get<string>());
					if (!inf) throw unknownElementException("deserializePrefab: unknown element");
					addElement(ent, inf, eleDesc["Init"]);
				}
			}
		}
		return res;
	}

	/*struct entityInfo {
		string name;
		int64_t id;
		int64_t parent;
		transform trans;
		vector<string> elementNames;
		vector<json> elementCreate;
	};*/

	vector<entRef> manager::deserializeScene(json const& data) {
		

		vector<entity*> allCreated;
		json::array_t remappedEntities;
		std::map<int64_t, entRef> idMap;
		entRef res;

		json::array_t ents = data["Entities"];

		if (ents.size() < 1) throw invalidPrefabException("empty tree");

		// check correctness
		for (int i = 0; i < ents.size(); i++) {
			json const& entDesc = ents[i];

			if (entDesc.find("Name") == entDesc.end() || !entDesc.find("Name")->is_string()) throw invalidPrefabException("missing or invalid prefab property 'Name'");
			if (entDesc.find("ID") == entDesc.end() || !entDesc.find("ID")->is_number_integer()) throw invalidPrefabException("missing or invalid prefab property 'ID'");
			if (entDesc.find("Parent") == entDesc.end() || !entDesc.find("Parent")->is_number_integer()) throw invalidPrefabException("missing or invalid prefab property 'Parent'");
			if (entDesc.find("Transform") == entDesc.end() || !isTransform(entDesc["Transform"])) throw invalidPrefabException("missing or invalid prefab property 'Transform'");
			if (entDesc.find("Elements") == entDesc.end() || !entDesc.find("Elements")->is_array()) {
				if (entDesc.find("Load") == entDesc.end() || !entDesc["Load"].is_string()) {
					throw invalidPrefabException("missing or invalid prefab property 'Elements' or 'Load'");
				} else {
					fpath p = ctcPath / entDesc["Load"].get<string>();
					string content = loadEntireFile(p.string());
					json j = json::parse(content);

				}
			} else {
				json::array_t const& el = entDesc["Elements"];

				for (int i = 0; i < el.size(); i++) {
					json const& elDesc = el[i];

					if (elDesc.find("Name") == elDesc.end() || !entDesc.find("Name")->is_string() || getInfoByName(elDesc["Name"]) == nullptr) throw invalidPrefabException("missing or unknown element name");
					if (elDesc.find("Init") == elDesc.end()) throw invalidPrefabException("missing prefab property 'Init' for elment type '" + elDesc["Name"].get<string>() + "'");
				}
			}
		}

		try {
			for (int i = 0; i < ents.size(); i++) {
				if (i == 0) {
					res = ealloc("");
					allCreated.push_back(res._ptr);
				} else {
					json entDesc = ents[i];

					// check correctness

					string name = entDesc["Name"].get<string>();
					int64_t id = entDesc["ID"].get<int64_t>();

					entRef er;

					if (entDesc.find("Load") == entDesc.end()) {
						er = ealloc(entDesc["Name"]);
					} else {
						string content = loadEntireFile(entDesc["Load"]);
						json j = json::parse(content);
						er = deserializeTree(j);

						int64_t id = entDesc["ID"].get<int64_t>();
						if (idMap.find(id) != idMap.end()) throw invalidPrefabHierarchyException("deserializePrefab: duplicate entity id");
						idMap[id] = er;
					}
				}
			}

			for (json const& entDesc : data["Entities"]) {
				remappedEntities.push_back(remapEleInitIDs(entDesc, idMap));
			}
		} catch (...) {
			throw invalidPrefabException("invalid json");
		}

		{
			for (json const& entDesc : remappedEntities) {
				string name = entDesc["Name"].get<string>();
				transform trans = loadTransform(entDesc["Transform"]);
				int64_t parentID = entDesc["Parent"].get<int64_t>();
				json::array_t const& elementsJson = entDesc["Elements"];

				entRef ent = idMap[entDesc["ID"].get<int64_t>()];
				if (parentID && idMap.find(parentID) == idMap.end()) throw invalidPrefabHierarchyException("deserializePrefab: parent entity not found");
				entRef parent = parentID ? idMap[parentID] : entRef();

				if (parent) setRelation(parent, ent);
				ent.setLocalTrans(trans);

				for (json const& eleDesc : elementsJson) {
					elementInfo* inf = getInfoByName(eleDesc["Name"].get<string>());
					if (!inf) throw unknownElementException("deserializePrefab: unknown element");
					addElement(ent, inf, eleDesc["Init"]);
				}
			}
		}
		return res;
	}

	manager::manager(fpath ctcPath) : ctcPath(ctcPath) {
		_frame = 0;
		_dt = 0.01;
		einit();
	}

	manager::~manager() {
		ecleanup();
	}
}