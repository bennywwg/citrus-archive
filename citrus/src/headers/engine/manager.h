#pragma once

#include <map>
#include <typeinfo>
#include <vector>
#include <typeindex>
#include <functional>
#include <shared_mutex>
#include <atomic>
#include <util/shared_recursive_mutex.h>

#include <engine/element.h>
#include <engine/entity.h>
#include <util/util.h>

namespace citrus {
	namespace engine {
		using std::vector;
		using std::function;
		using std::tuple;
		using std::pair;
		using std::string;
		using std::type_info;
		using std::type_index;

		class manager;
		class engine;

		class eleInitBase {
			public:
			const type_index type;
			const json data;
			public:
			inline eleInitBase(type_index type, json data) : type(type), data(data) { }
		};
		template<class T>
		class eleInit : public eleInitBase {
			public:
			inline eleInit(json data) : eleInitBase(typeid(T), data) { }
		};


		class manager {
			
			friend class entity;

			engine* const _eng;

			struct elementInfo {
				function<void(element*, entity*)> ctor;
				function<void(element*)> dtor;
				vector<tuple<element*, entity*, json>> toCreate;
				vector<pair<element*, entity*>> toDestroy;
				vector<element*> existing;
				vector<entity*> existingEntities;
				int size;
				string name;
				type_index type;

				inline int findInToCreate(element* ele) {
					for(int i = 0; i < toCreate.size(); ++i)
						if(std::get<0>(toCreate[i]) == ele)
							return i;

					return -1;
				}
				inline int findInToDestroy(element* ele) {
					for(int i = 0; i < toDestroy.size(); ++i)
						if(toDestroy[i].first == ele)
							return i;

					return -1;
				}
				inline int findInExisting(element* ele) {
					for(int i = 0; i < existing.size(); ++i)
						if(existing[i] == ele)
							return i;

					return -1;
				}
				inline int findInExistingEntities(entity* ent) {
					for(int i = 0; i < existingEntities.size(); ++i)
						if(existingEntities[i] == ent)
							return i;

					return -1;
				}

				inline elementInfo() : type(typeid(void)) { }
			};
			std::map<type_index, elementInfo> _data;

			vector<type_index> _order;

			private: vector<entity*> _toCreate;
			public: std::recursive_mutex toCreateMut;

			private: vector<entity*> _toDestroy;
			public: std::recursive_mutex toDestroyMut;

			private: vector<entity*> _entities;
			public: std::recursive_mutex entitiesMut;

			private:
			inline int findEntity(const vector<entity*>& entities, entity* ent) {
				for(int i = 0; i < entities.size(); ++i)
					if(entities[i] == ent)
						return i;

				return -1;
			}

			//serialization functions
			

			#pragma region(type support functions)
			private:
			//gets the elementInfo for a type if it exists, null if it doesn't exist
			inline elementInfo* getInfo(const type_index& index) {
				auto it = _data.find(index);
				if(it != _data.end()) {
					return &(it->second);
				} else {
					return nullptr;
				}
			}
			//gets the elementInfo for a given type name, null if it doesn't exist
			inline elementInfo* getInfoByName(const string& name) {
				//TODO: Optimize this (with another map)
				for(auto& it : _data) {
					if(it.second.name == name) {
						return &it.second;
					}
				}
				return nullptr;
			}

			//returns true if a vector of types 
			static inline bool containsType(const vector<type_index>& types, const type_index& type) {
				for(const auto& t : types)
					if(t == type)
						return true;
				return false;
			}
			static inline int containsType(const vector<eleInitBase>& infos, const type_index& type) {
				for(int i = 0; i < infos.size(); i++) {
					const auto& info = infos[i];
					if(info.type == type)
						return i;
				}

				return -1;
			}

			inline vector<type_index> reorder(const vector<type_index>& types) {
				vector<type_index> res;

				for(const auto& type : _order)
					if(containsType(types, type))
						res.push_back(type);

				return res;
			}
			inline vector<eleInitBase> reorder(const vector<eleInitBase>& infos) {
				vector<eleInitBase> res;

				for(const auto& type : _order) {
					int index = containsType(infos, type);
					if(index != -1)
						res.push_back(infos[index]);
				}

				return res;
			}
			#pragma endregion


			#pragma region(type registration functions)
			//register a type so it is usable
			public: template<class T> inline void registerType(string name) {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				const auto& index = type_index(typeid(T));
				auto it = _data.find(index);
				if(it == _data.end()) {
					elementInfo& info = _data[index];
					info.ctor = function<void(element*, entity*)>([](element* loc, entity* ent) { new(loc) T(ent); });
					info.dtor = function<void(element*)>([](element* loc) { ((T*)loc)->~T(); });
					info.size = sizeof(T);
					info.type = index;
					info.name = name;
				}
			}
			//set the order in which scipts will be executed
			//later types depend on previous types
			//during initialization and runtime, this happens in the order specified
			//during destruction, this happens in reverse order specified
			public: inline void setOrder(const vector<type_index>& order) {
				_order.clear();
				_order.reserve(order.size());
				for(unsigned int i = 0; i < order.size(); ++i) {
					for(unsigned int u = 0; u < _order.size(); ++u)
						if(_order[u] == order[i])
							throw std::exception("manager can only have one of each type in order");
					_order.push_back(order[i]);
				}
			}
			#pragma endregion
			

			#pragma region(creation deletion support functions)
			private:
			inline entity* newEntity(string name, const vector<eleInitBase>& orderedData, uint64_t uuid) {
				vector<elementMeta> types;
				types.reserve(orderedData.size());
				for(const auto& info : orderedData) {
					const auto& traits = getInfo(info.type);
					types.emplace_back(info.type, (element*)(::operator new(traits->size)));
				}

				return new entity(types, _eng, name, uuid);
			}
			#pragma endregion


			#pragma region(creation deletion functions)
			public:
			//queue an object for creation
			//this object is valid until destroy() is called for it flush() is called, at which point its memory is freed
			//!!! hard locks toCreateMut !!!
			inline entity* create(string name, const vector<eleInitBase>& data, uint64_t id) {
				std::lock_guard<std::recursive_mutex> lock(toCreateMut);
				return createUnsafe(name, data, id);
			}
			//queue an object for creation
			//this object is valid until destroy() is called for it flush() is called, at which point its memory is freed
			//!!! must hard lock toCreateMut !!!
			inline entity* createUnsafe(string name, const vector<eleInitBase>& data, uint64_t id) {
				auto orderedData = reorder(data);

				auto ent = newEntity(name, orderedData, id);

				for(int i = 0; i < ent->_elements.size(); ++i)
					getInfo(ent->_elements[i].type)->toCreate.emplace_back(ent->_elements[i].ele, ent, orderedData[i].data);

				_toCreate.push_back(ent);

				return ent;
			}

			//queue an object for deletion
			//the object is still valid until flush() is called, at which point its memory is freed
			//!!! hard locks toCreateMut and toDestroMut !!!
			inline void destroy(entity* obj) {
				std::lock_guard<std::recursive_mutex> lock0(toCreateMut), lock1(toDestroyMut);
				destroyUnsafe(obj);
			}
			//queue an object for deletion
			//the object is still valid until flush() is called, at which point its memory is freed
			//!!! must hard lock toCreateMut and toDestroyMut !!!
			inline void destroyUnsafe(entity* ent) {
				//try and remove ent from the _toCreate list if its in it
				int entIndex = findEntity(_toCreate, ent);
				if(entIndex != -1) {
					//if ent is in _toCreate, also remove the elements from their lists
					for(auto& meta : ent->_elements) {
						auto& info = *getInfo(meta.type);

						int eleIndex = info.findInToCreate(meta.ele);
						if(eleIndex != -1) {
							::operator delete(meta.ele);
							info.toCreate.erase(info.toCreate.begin() + eleIndex);
						} else {
							throw std::exception("Couldn't find a certain element that should exist");
						}
					}

					_toCreate.erase(_toCreate.begin() + entIndex);

					delete ent;

					return;
				}

				//if it's already in _toDestroy, just do nothing
				if(findEntity(_toDestroy, ent) != -1) return;

				//otherwise add it and all of its elements to _toDestroy
				_toDestroy.push_back(ent);
				for(auto& meta : ent->_elements)
					getInfo(meta.type)->toDestroy.emplace_back(meta.ele, ent);
			}
			#pragma endregion

			
			#pragma region(querying functions)

			//
			template<class T> inline vector<T*> ofType() {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				auto pre = ofType(type_index(typeid(T)));
				vector<T*> res;
				res.reserve(pre.size());
				for(auto ptr : pre)
					res.push_back((T*)ptr);
				return res;
			}
			
			//
			inline vector<element*> ofType(const type_index& index) {
				return getInfo(index)->existing;
			}

			//
			inline vector<entity*> allEntities() {
				return _entities;
			}

			inline entity* findByIDUnsafe(uint64_t uuid) {
				if(uuid == entity::nullID) return nullptr;
				for(auto ent : _entities)
					if(ent->id == uuid)
						return ent;
				for(auto ent : _toCreate)
					if(ent->id == uuid)
						return ent;
				return nullptr;
			}
			inline entity* findByID(uint64_t uuid) {
				std::lock_guard<std::recursive_mutex> lock(toCreateMut), lock1(entitiesMut);
				return findByIDUnsafe(uuid);
			}

			#pragma endregion
			

			#pragma region(serialization support functions)

			//scans through an eleInitBase and replaces all
			//entity and element references, mapping the IDs
			//using the given id map
			private: inline eleInitBase remapEleInitIDs(eleInitBase info, const std::map<uint64_t, uint64_t>& remappedIDs) {
				json remappedData = info.data;
				util::recursive_iterate(remappedData, [this, &remappedIDs](json::iterator it) {
					if(this->isEntityReference(*it) || this->isElementReference(*it)) {
						auto found = remappedIDs.find((*it)["ID"].get<uint64_t>());
						if(found != remappedIDs.end()) {
							(*it)["ID"] = (*found).second;
						} else {
							throw std::exception("did not find mapped ID");
						}
					}
				});
				return eleInitBase(info.type, remappedData);
			}
			
			public:
			inline json referenceEntity(entity* ent) {
				return json({
					{"Type", "Entity Reference"},
					{"ID", ent != nullptr ? ent->id : entity::nullID}
					});
			}
			template<class T> inline json referenceElement(entity* ent) {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				auto info = getInfo(typeid(T));
				if(info == nullptr) throw std::exception("Trying to reference element type that isn't registered");
				return json({
					{"Type", "Element Reference"},
					{"Name", info->name},
					{"ID", ent != nullptr ? ent->id : entity::nullID}
					});
			}
			inline entity* dereferenceEntity(const json& data) {
				if(!isEntityReference(data)) throw std::exception(("Tried to derefence invalid entity reference\n" + data.dump(2)).c_str());
				return findByID(data["ID"].get<uint64_t>());
			}
			template<class T> inline T* dereferenceElement(const json& data) {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				if(!isElementReference(data)) throw std::exception(("Tried to derefence invalid element reference\n" + data.dump(2)).c_str());
				auto info = getInfo(typeid(T));
				if(data["Name"] != info->name) throw std::exception(("Tried to derefence element but the template type does not match the json\n" + data.dump(2)).c_str());
				auto ent = findByID(data["ID"].get<uint64_t>());
				if(ent == nullptr) throw std::exception("Tried to dereference element but its entity does not exist");
				if(!ent->initialized()) throw std::exception("Tried to dereference element but its entity is not initialized");
				return ent->getElement<T>();
			}
			inline bool isEntityReference(const json& data) {
				auto foundType = data.find("Type");
				if(foundType == data.end() || !foundType.value().is_string() || foundType.value().get<string>() != "Entity Reference") return false;
				auto foundID = data.find("ID");
				if(foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
				return true;
			}
			inline bool isElementReference(const json& data) {
				auto foundType = data.find("Type");
				if(foundType == data.end() || !foundType.value().is_string() || foundType.value().get<string>() != "Element Reference") return false;
				auto foundName = data.find("Name");
				if(foundName == data.end() || !foundName.value().is_string() || getInfoByName(foundName.value().get<string>()) == nullptr) return false;
				auto foundID = data.find("ID");
				if(foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
				return true;
			}

			#pragma endregion


			#pragma region(serialization functions)

			public:
			//creates all the entities described in a prefab
			//you must lock toCreateMut and entitiesMut or risk UB
			inline void loadPrefabUnsafe(const json& data) {
				std::map<uint64_t, uint64_t> remappedIDs;

				json dataEntities = data["Entities"];
				for(int i = 0; i < dataEntities.size(); i++)
					remappedIDs[dataEntities[i]["ID"].get<uint64_t>()] = util::nextID();

				vector<pair<entity*, uint64_t>> parentMap;
				for(int i = 0; i < dataEntities.size(); i++) {
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
					ent->setLocalTransform(trans);
					parentMap.emplace_back(ent, parent);
				}

				for(const auto& kvp : parentMap) {
					kvp.first->_parent = findByIDUnsafe(kvp.second);
				}
			}
			inline void loadPrefab(const json& data) {
				std::lock_guard<std::recursive_mutex> lock0(toCreateMut), lock1(entitiesMut);
				loadPrefabUnsafe(data);
			}
			//saves all the entities in the hierarchy of toSave
			//you must lock entitiesMut or risk UB
			inline json savePrefabUnsafe(entity* toSave) {
				if(toSave == nullptr || !toSave->initialized()) throw std::exception("Cannot save null or uninitialized entity");
				vector<entity*> connected = toSave->getAllConnected();
				json res;
				res["Entities"] = { };
				for(entity* ent : connected) {
					json entElements;
					for(auto& ele : ent->_elements) {
						entElements[getInfo(ele.type)->name] = ele.ele->save();
					}

					json entDescriptor({
						{"Name", ent->name},
						{"ID", ent->id},
						{"Parent", ent->_parent == nullptr ? entity::nullID : ent->_parent->id},
						{"Transform", util::save(ent->getLocalTransform())},
						{"Elements", entElements}
						});

					res["Entities"].push_back(entDescriptor);
				}
				return res;
			}
			inline json savePrefab(entity* toSave) {
				std::lock_guard<std::recursive_mutex> lock(entitiesMut);
				return savePrefabUnsafe(toSave);
			}

			#pragma endregion


			#pragma region(runtime execution functions)

			public:
			//creates all objects queued for creation and deletes all queued for destruction
			//!!! locks dataMut and objectMut !!!
			inline void flush() {
				std::lock_guard<std::recursive_mutex> lock(toCreateMut), lock1(toDestroyMut), lock2(entitiesMut);

				flushToDestroyUnsafe();

				flushToCreateUnsafe();
			}

			inline void render() {
				for(const auto& oType : _order)
					for(auto& ele : getInfo(oType)->existing)
						ele->render();
			}

			inline void preRender() {
				for(const auto& oType : _order)
					for(auto& ele : getInfo(oType)->existing)
						ele->preRender();
			}

			private:
			inline void flushToCreateUnsafe() {
				//initialize all elements in order
				for(const auto& oType : _order) {
					auto& info = *getInfo(oType);

					for(auto& meta : info.toCreate) {
						info.ctor(std::get<0>(meta), std::get<1>(meta));
						info.existing.push_back(std::get<0>(meta));
						info.existingEntities.push_back(std::get<1>(meta));
					}
				}

				//add newly created entities
				for(const auto& ent : _toCreate) {
					_entities.push_back(ent);
					ent->_initialized = true;
				}

				//clear entity toCreate list
				_toCreate.clear();

				//call onCreate for all elements in order
				for(const auto& oType : _order) {
					auto& info = *getInfo(oType);

					for(auto& meta : info.toCreate) {
						std::get<0>(meta)->load(std::get<2>(meta));
					}
				}

				//clear all element toCreate lists
				for(const auto& oType : _order) {
					auto& info = *getInfo(oType);

					info.toCreate.clear();
				}
			}

			inline void flushToDestroyUnsafe() {
				//remove entities from scene
				for(const auto& ent : _toDestroy) {
					int entIndex = findEntity(_entities, ent);
					if(entIndex != -1) {
						_entities.erase(_entities.begin() + entIndex);
					} else {
						throw std::exception("failed to find entity in _entities to destroy");
					}
				}

				//call dtors on elements in order	
				for(const auto& oType : _order) {
					auto& info = *getInfo(oType);

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
				}

				//clear all element toDestroy lists
				for(const auto& oType : _order) {
					auto& info = *getInfo(oType);

					info.toDestroy.clear();
				}

				//deallocate memory for all entities
				for(const auto& ent : _toDestroy) {
					delete ent;
				}

				//clear entity toDestroy list
				_toDestroy.clear();
			}

			#pragma endregion
			

			public:
			inline manager(engine* eng) : _eng(eng) { }
			inline ~manager() {
				//throw std::exception("just a reminder you have to write this still");
			}

			private:
			manager(const manager& other) = delete;
			manager& operator=(const manager& other) = delete;
		};
	}
}