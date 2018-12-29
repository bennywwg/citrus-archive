#include <engine/manager.h>
#include <engine/elements/renderManager.h>
#include <engine/elements/freeCam.h>
#include <engine/elementRef.inl>

namespace citrus {
	namespace engine {
		int manager::findEntity(const vector<shared_ptr<entity>>& entities, const shared_ptr<entity>& ent) {
			for(uint32_t i = 0; i < entities.size(); ++i)
				if(entities[i] == ent)
					return i;

			return -1;
		}

		manager::elementInfo* manager::getInfo(const type_index & index) {
			auto it = _data.find(index);
			if(it != _data.end()) {
				return &it->second;
			} else {
				throw std::runtime_error(string("manager does not have type ") + index.name());
			}
		}

		manager::elementInfo* manager::getInfoByName(const string & name) {
			//TODO: Optimize this (with another map)
			for(auto& it : _data) {
				if(it.second.name == name) {
					return &it.second;
				}
			}
			throw std::runtime_error(string("manager does not have type ") + name);
		}

		int manager::containsType(const vector<eleInitBase>& infos, const type_index & type) {
			for(uint32_t i = 0; i < infos.size(); i++) {
				const auto& info = infos[i];
				if(info.type == type)
					return i;
			}

			return -1;
		}

		vector<eleInitBase> manager::reorder(const vector<eleInitBase>& infos) {
			vector<eleInitBase> res;

			for(const auto& type : _order) {
				int index = containsType(infos, type);
				if(index != -1)
					res.push_back(infos[index]);
			}

			return res;
		}

		void manager::setOrder(const vector<type_index>& order) {
			std::lock_guard<std::recursive_mutex> lock0(toCreateMut), lock1(toDestroyMut), lock2(entitiesMut);
			_order.clear();
			_order.reserve(order.size());
			for(unsigned int i = 0; i < order.size(); ++i) {
				for(unsigned int u = 0; u < _order.size(); ++u)
					if(_order[u] == order[i])
						throw std::runtime_error("manager can only have one of each type in order");
				_order.push_back(order[i]);
			}
		}

		// GOOD !
		entityRef manager::create(string name, const vector<eleInitBase>& data, uint64_t id) {
			std::lock_guard<std::recursive_mutex> lock(toCreateMut);
			return createUnsafe(name, data, id);
		}

		// GOOD !
		entityRef manager::createUnsafe(string name, const vector<eleInitBase>& data, uint64_t id) {
			auto orderedData = reorder(data);

			vector<element*> eles;
			eles.reserve(orderedData.size());
			for(const auto& info : orderedData) {
				const auto& traits = getInfo(info.type);
				//TODO: allocate all next to each other in memory
				eles.emplace_back((element*)(::operator new(traits->size)));
			}

			std::shared_ptr<entity> sp(new entity(eles, _eng, name, id));

			for(uint32_t i = 0; i < eles.size(); ++i)
				getInfo(orderedData[i].type)->toCreate.emplace_back(eles[i], entityRef(sp), orderedData[i].data, orderedData[i].init);

			_toCreate.push_back(sp);

			return entityRef(sp);
		}

		// GOOD !
		void manager::destroy(entityRef ent) {
			std::lock_guard<std::recursive_mutex> lock0(toCreateMut), lock1(toDestroyMut);
			destroyUnsafe(ent);
		}

		// GOOD !
		void manager::destroyUnsafe(entityRef ent) {
			entity* rent = ent._ref.lock().get();

			//try and remove ent from the _toCreate list if its in it
			int entIndex = findEntity(_toCreate, ent._ref.lock());
			if(entIndex != -1) {
				//if ent is in _toCreate, also remove the elements from their lists
				for(element* ele : rent->_elements) {
					auto& info = *getInfo(ele->_type);

					int eleIndex = info.findInToCreate(ele);
					if(eleIndex != -1) {
						::operator delete(ele);
						info.toCreate.erase(info.toCreate.begin() + eleIndex);
					} else {
						throw std::runtime_error("Couldn't find a certain element that should exist");
					}
				}

				_toCreate.erase(_toCreate.begin() + entIndex);

				return;
			}

			//if it's already in _toDestroy, just do nothing
			if(findEntity(_toDestroy, ent._ref.lock()) != -1) return;

			//otherwise add it and all of its elements to _toDestroy
			_toDestroy.push_back(ent._ref.lock());
			for(element* ele : rent->_elements)
				getInfo(ele->_type)->toDestroy.emplace_back(ele, ent);
		}

		vector<element*> manager::ofType(const type_index & index) {
			std::lock_guard<std::recursive_mutex> lock0(entitiesMut);
			return getInfo(index)->existing;
		}

		vector<entityRef> manager::allEntities() {
			std::lock_guard<std::recursive_mutex> lock(entitiesMut);
			vector<entityRef> res; res.reserve(_entities.size());
			for(auto& sp : _entities) res.push_back(entityRef(sp));
			return res;
		}

		entityRef manager::findByID(uint64_t uuid) {
			std::lock_guard<std::recursive_mutex> lock0(entitiesMut), lock1(toCreateMut);
			if(uuid == entity::nullID) return entityRef();
			for(shared_ptr<entity>& ent : _entities)
				if(ent->id == uuid)
					return entityRef(ent);
			for(shared_ptr<entity>& ent : _toCreate)
				if(ent->id == uuid)
					return entityRef(ent);
			return entityRef();
		}

		eleInitBase manager::remapEleInitIDs(eleInitBase info, const std::map<uint64_t, uint64_t>& remappedIDs) {
			json remappedData = info.data;
			util::recursive_iterate(remappedData, [this, &remappedIDs](json::iterator it) {
				if(this->isEntityReference(*it) || this->isElementReference(*it)) {
					auto found = remappedIDs.find((*it)["ID"].get<uint64_t>());
					if(found != remappedIDs.end()) {
						(*it)["ID"] = (*found).second;
					} else {
						throw std::runtime_error("did not find mapped ID");
					}
				}
			});
			return eleInitBase(info.type, remappedData);
		}

		json manager::referenceEntity(entityRef ent) {
			return json({
				{"Type", "Entity Reference"},
				{"ID", ent != nullptr ? ent.id() : entity::nullID}
			});
		}

		entityRef manager::dereferenceEntity(const json & data) {
			if(!isEntityReference(data)) throw std::runtime_error(("Tried to derefence invalid entity reference\n" + data.dump(2)).c_str());
			return findByID(data["ID"].get<uint64_t>());
		}

		bool manager::isEntityReference(const json & data) {
			auto foundType = data.find("Type");
			if(foundType == data.end() || !foundType.value().is_string() || foundType.value().get<string>() != "Entity Reference") return false;
			auto foundID = data.find("ID");
			if(foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
			return true;
		}

		bool manager::isElementReference(const json & data) {
			auto foundType = data.find("Type");
			if(foundType == data.end() || !foundType.value().is_string() || foundType.value().get<string>() != "Element Reference") return false;
			auto foundName = data.find("Name");
			if(foundName == data.end() || !foundName.value().is_string() || getInfoByName(foundName.value().get<string>()) == nullptr) return false;
			auto foundID = data.find("ID");
			if(foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
			return true;
		}

		void manager::loadPrefabUnsafe(const json & data) {
			std::map<uint64_t, uint64_t> remappedIDs;

			json dataEntities = data["Entities"];
			for(uint32_t i = 0; i < dataEntities.size(); i++)
				remappedIDs[dataEntities[i]["ID"].get<uint64_t>()] = util::nextID();

			vector<pair<entityRef, uint64_t>> parentMap;
			for(uint32_t i = 0; i < dataEntities.size(); i++) {
				json entDesc = dataEntities[i];
				string name = entDesc["Name"].get<string>();
				uint64_t id = remappedIDs[entDesc["ID"].get<uint64_t>()];
				uint64_t parent = entDesc["Parent"].is_null() ? entity::nullID : remappedIDs[entDesc["Parent"].get<uint64_t>()];
				transform trans = util::loadTransform(entDesc["Transform"]);

				vector<eleInitBase> elements;
				json elementsJson = entDesc["Elements"];
				for(auto it = elementsJson.begin(); it != elementsJson.end(); ++it) {
					elements.push_back(remapEleInitIDs(eleInitBase(getInfoByName(it.key())->type, it.value()), remappedIDs));
				}

				auto ent = createUnsafe(name, elements, id);
				ent.setLocalTransform(trans);
				parentMap.emplace_back(ent, parent);
			}

			for(const auto& kvp : parentMap) {
				kvp.first.setParent(findByID(kvp.second));
			}
		}

		void manager::loadPrefab(const json & data) {
			std::lock_guard<std::recursive_mutex> lock0(entitiesMut), lock1(toCreateMut);
			loadPrefabUnsafe(data);
		}

		json manager::savePrefabUnsafe(entityRef toSave) {
			if(toSave == nullptr) throw std::runtime_error("Cannot save null or uninitialized entity");
			vector<entityRef> connected = toSave.getAllConnected();
			json res;
			res["Entities"] = { };
			for(entityRef ent : connected) {
				json entElements;
				for(element* ele : ent._ref.lock().get()->_elements) {
					entElements[getInfo(ele->_type)->name] = ele->save();
				}

				json entDescriptor({
					{"Name", ent.name()},
					{"ID", ent.id()},
					{"Parent", ent.getParent() == nullptr ? entity::nullID : ent.getParent().id()},
					{"Transform", util::save(ent.getLocalTransform())},
					{"Elements", entElements}
					});

				res["Entities"].push_back(entDescriptor);
			}
			return res;
		}

		json manager::savePrefab(entityRef toSave) {
			std::lock_guard<std::recursive_mutex> lock(entitiesMut);
			return savePrefabUnsafe(toSave);
		}

		void manager::flush() {
			std::lock_guard<std::recursive_mutex> lock(toCreateMut), lock1(toDestroyMut), lock2(entitiesMut);

			flushToDestroyUnsafe();

			flushToCreateUnsafe();
		}

		void manager::render() {
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);
				auto begin = std::chrono::high_resolution_clock::now();
				if(info.active)
					for(auto& ele : info.existing)
						ele->render();
				auto end = std::chrono::high_resolution_clock::now();
				info.stats.render = (end - begin);
			}
		}

		void manager::preRender() {
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);
				auto begin = std::chrono::high_resolution_clock::now();
				if(info.active)
					for(auto& ele : info.existing)
						ele->preRender();
				auto end = std::chrono::high_resolution_clock::now();
				info.stats.preRender = end - begin;
			}
		}

		void manager::flushToCreateUnsafe() {
			//initialize all elements in order
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);

				auto begin = std::chrono::high_resolution_clock::now();

				for(auto& meta : info.toCreate) {
					info.ctor(std::get<0>(meta), std::get<1>(meta));
					info.existing.push_back(std::get<0>(meta));
					info.existingEntities.push_back(std::get<1>(meta));
					//if(std::get<3>(meta)) std::get<3>(meta)(std::get<0>(meta));
					std::get<0>(meta)->_initialized = true;
				}

				auto end = std::chrono::high_resolution_clock::now();

				info.stats.ctor = end - begin;
			}

			//add newly created entities
			for(shared_ptr<entity>& ent : _toCreate) {
				_entities.push_back(ent);
			}

			//clear entity toCreate list
			_toCreate.clear();

			//call onCreate for all elements in order
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);

				auto begin = std::chrono::high_resolution_clock::now();

				for(auto& meta : info.toCreate) {
					std::get<0>(meta)->load(std::get<2>(meta));
				}

				auto end = std::chrono::high_resolution_clock::now();

				info.stats.load = end - begin;
			}

			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);

				for(auto& meta : info.toCreate) {
					if(std::get<3>(meta)) std::get<3>(meta)(std::get<0>(meta));
					//std::get<0>(meta)->_initialized = true;
				}

			}


			//clear all element toCreate lists
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);

				info.toCreate.clear();
			}
		}

		void manager::flushToDestroyUnsafe() {
			//remove entities from scene
			for(const auto& ent : _toDestroy) {
				int entIndex = findEntity(_entities, ent);
				if(entIndex != -1) {
					_entities.erase(_entities.begin() + entIndex);
				} else {
					throw std::runtime_error("failed to find entity in _entities to destroy");
				}
			}

			//call dtors on elements in order
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);

				auto begin = std::chrono::high_resolution_clock::now();

				for(auto& meta : info.toDestroy) {
					//remove from existing entity list
					for(unsigned int i = 0; i < info.existingEntities.size(); ++i)
						if(meta.second == info.existingEntities[i])
							info.existingEntities.erase(info.existingEntities.begin() + i);

					//remove from existing element list
					for(unsigned int i = 0; i < info.existing.size(); ++i)
						if(std::get<0>(meta) == info.existing[i])
							info.existing.erase(info.existing.begin() + i);


					info.dtor(std::get<0>(meta));
					::operator delete(std::get<0>(meta));
				}

				auto end = std::chrono::high_resolution_clock::now();

				info.stats.dtor = end - begin;
			}

			//clear all element toDestroy lists
			for(const auto& oType : _order) {
				auto& info = *getInfo(oType);

				info.toDestroy.clear();
			}

			//deallocate memory for all entities and ...
			//clear entity toDestroy list
			_toDestroy.clear();
		}

		vector<type_index> manager::reorder(const vector<type_index>& types) {
			vector<type_index> res;

			for(const auto& type : _order)
				if(containsType(types, type))
					res.push_back(type);

			return res;
		}
		
		bool manager::containsType(const vector<type_index>& types, const type_index & type) {
			for(const auto& t : types)
				if(t == type)
					return true;
			return false;
		}

		manager::manager(engine * eng) : _eng(eng) {
		}
		manager::~manager() {
			//throw std::runtime_error("You need to implement this");
		}
	}
}