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
		class manager;
		class engine;

		class eleInitBase {
			public:
			const std::type_index type;
			const nlohmann::json data;
			public:
			inline eleInitBase(std::type_index type, nlohmann::json data) : type(type), data(data) { }
		};
		template<class T>
		class eleInit : public eleInitBase {
			public:
			inline eleInit(nlohmann::json data) : eleInitBase(typeid(T), data) { }
		};

		class manager {
			engine* const _eng;

			struct elementInfo {
				std::function<void(element*, entity*)> ctor;
				std::function<void(element*)> dtor;
				std::vector<std::tuple<element*, entity*, nlohmann::json>> toCreate;
				std::vector<std::pair<element*, entity*>> toDestroy;
				std::vector<element*> existing;
				std::vector<entity*> existingEntities;
				int size;
				std::string name;
				std::type_index type;

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
			std::map<std::type_index, elementInfo> _data;

			std::vector<std::type_index> _order;

			private:
			inline int findEntity(const std::vector<entity*>& entities, entity* ent) {
				for(int i = 0; i < entities.size(); ++i)
					if(entities[i] == ent)
						return i;

				return -1;
			}


			public:
			inline nlohmann::json referenceEntity(entity* ent) {
				return nlohmann::json({
					{"Type", "Entity Reference"},
					{"ID", ent != nullptr ? ent->id : entity::nullID}
				});
			}
			template<class T> inline nlohmann::json referenceElement(entity* ent) {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				auto info = getInfo(typeid(T));
				if(info == nullptr) throw std::exception("Trying to reference element type that isn't registered");
				return nlohmann::json({
					{"Type", "Element Reference"},
					{"Name", info->name},
					{"ID", ent != nullptr ? ent->id : entity::nullID}
				});
			}
			inline entity* dereferenceEntity(const nlohmann::json& data) {
				if(!isEntityReference(data)) throw std::exception(("Tried to derefence invalid entity reference\n" + data.dump(2)).c_str());
				return findByID(data["ID"].get<uint64_t>());
			}
			template<class T> inline T* dereferenceElement(const nlohmann::json& data) {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				if(!isElementReference(data)) throw std::exception(("Tried to derefence invalid element reference\n" + data.dump(2)).c_str());
				auto info = getInfo(typeid(T));
				if(data["Name"] != info->name) throw std::exception(("Tried to derefence element but the template type does not match the json\n" + data.dump(2)).c_str());
				auto ent = findByID(data["ID"].get<uint64_t>());
				if(ent == nullptr) throw std::exception("Tried to dereference element but its entity does not exist");
				if(!ent->initialized()) throw std::exception("Tried to dereference element but its entity is not initialized");
				return ent->getElement<T>();
			}
			inline bool isEntityReference(const nlohmann::json& data) {
				auto foundType = data.find("Type");
				if(foundType == data.end() || !foundType.value().is_string() || foundType.value().get<std::string>() != "Entity Reference") return false;
				auto foundID = data.find("ID");
				if(foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
				return true;
			}
			inline bool isElementReference(const nlohmann::json& data) {
				auto foundType = data.find("Type");
				if(foundType == data.end() || !foundType.value().is_string() || foundType.value().get<std::string>() != "Element Reference") return false;
				auto foundName = data.find("Name");
				if(foundName == data.end() || !foundName.value().is_string() || getInfoByName(foundName.value().get<string>()) == nullptr) return false;
				auto foundID = data.find("ID");
				if(foundID == data.end() || foundID.value().is_null() || !foundID.value().is_number_unsigned()) return false;
				return true;
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

			private: std::vector<entity*> _toCreate;
			public: std::recursive_mutex toCreateMut;

			private: std::vector<entity*> _toDestroy;
			public: std::recursive_mutex toDestroyMut;

			private: std::vector<entity*> _entities;
			public: std::recursive_mutex entitiesMut;

			private:
			//gets the elementInfo for a type if it exists
			inline elementInfo* getInfo(const std::type_index& index) {
				auto it = _data.find(index);
				if(it != _data.end()) {
					return &(it->second);
				} else {
					return nullptr;
				}
			}
			inline elementInfo* getInfoByName(const std::string& name) {
				//TODO: Optimize this (with another map)
				for(auto it : _data) {
					if(it.second.name == name) {
						return &it.second;
					}
				}
				return nullptr;
			}

			inline bool containsType(const std::vector<std::type_index>& types, const std::type_index& type) {
				for(const auto& t : types)
					if(t == type)
						return true;
				return false;
			}
			inline int containsType(const std::vector<eleInitBase>& infos, const std::type_index& type) {
				for(int i = 0; i < infos.size(); i++) {
					const auto& info = infos[i];
					if(info.type == type)
						return i;
				}
					
				return -1;
			}

			inline std::vector<std::type_index> reorder(const std::vector<std::type_index>& types) {
				std::vector<std::type_index> res;

				for(const auto& type : _order)
					if(containsType(types, type))
						res.push_back(type);

				return res;
			}
			inline std::vector<eleInitBase> reorder(const std::vector<eleInitBase>& infos) {
				std::vector<eleInitBase> res;

				for(const auto& type : _order) {
					int index = containsType(infos, type);
					if(index != -1)
						res.push_back(infos[index]);
				}

				return res;
			}

			inline entity* newEntity(std::string name, const std::vector<eleInitBase>& orderedData, uint64_t uuid) {
				std::vector<elementMeta> types;
				types.reserve(orderedData.size());
				for(const auto& info : orderedData) {
					const auto& traits = getInfo(info.type);
					types.emplace_back(info.type, (element*)(::operator new(traits->size)));
				}

				return new entity(types, _eng, name, uuid);
			}

			public:
			//lock this to prevent your underlying objects from being deleted
			std::recursive_mutex objectMut;

			//makes an element type usable
			template<class T> inline void registerType(std::string name) {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				const auto& index = std::type_index(typeid(T));
				auto it = _data.find(index);
				if(it == _data.end()) {
					elementInfo& info = _data[index];
					info.ctor = std::function<void(element*, entity*)>([](element* loc, entity* ent) { new(loc) T(ent); });
					info.dtor = std::function<void(element*)>([](element* loc) { ((T*)loc)->~T(); });
					info.size = sizeof(T);
					info.type = index;
					info.name = name;
				}
			}
			inline void setOrder(const std::vector<std::type_index>& order) {
				_order.clear();
				_order.reserve(order.size());
				for(unsigned int i = 0; i < order.size(); ++i) {
					for(unsigned int u = 0; u < _order.size(); ++u)
						if(_order[u] == order[i])
							throw std::exception("manager can only have one of each type in order");
					_order.push_back(order[i]);
				}
			}

			//returns a vector of all the objects with given type
			//!!! soft locks dataMut !!!
			template<class T> inline std::vector<T*> ofType() {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				auto pre = ofType(std::type_index(typeid(T)));
				std::vector<T*> res;
				res.reserve(pre.size());
				for(auto ptr : pre)
					res.push_back((T*)ptr);
				return res;
			}
			//returns a vector of all the objects with given type
			//!!! soft locks dataMut !!!
			inline std::vector<element*> ofType(const std::type_index& index) {
				return getInfo(index)->existing;
			}
			inline std::vector<entity*> allEntities() {
				return _entities;
			}

			//queue an object for creation
			//this object is valid until destroy() is called for it flush() is called, at which point its memory is freed
			//!!! hard locks toCreateMut !!!
			inline entity* create(std::string name, const std::vector<eleInitBase>& data, uint64_t id) {
				std::lock_guard<std::recursive_mutex> lock(toCreateMut);
				return createUnsafe(name, data, id);
			}
			//queue an object for creation
			//this object is valid until destroy() is called for it flush() is called, at which point its memory is freed
			//!!! must hard lock toCreateMut !!!
			inline entity* createUnsafe(std::string name, const std::vector<eleInitBase>& data, uint64_t id) {
				auto orderedData = reorder(data);

				auto ent = newEntity(name, orderedData, id);
				
				for(int i = 0; i < ent->_elements.size(); ++i)
					getInfo(ent->_elements[i].type)->toCreate.emplace_back(ent->_elements[i].ele, ent, orderedData[i].data);

				_toCreate.push_back(ent);

				return ent;
			}

			inline void loadPrefabUnsafe(const nlohmann::json& data) {
				std::map<uint64_t, uint64_t> remappedIDs;

				json dataEntities = data["Entities"];
				for(int i = 0; i < dataEntities.size(); i++)
					remappedIDs[dataEntities[i]["ID"].get<uint64_t>()] = util::nextID();

				std::vector<std::pair<entity*, uint64_t>> parentMap;
				for(int i = 0; i < dataEntities.size(); i++) {
					json entDesc = dataEntities[i];
					std::string name = entDesc["Name"].get<std::string>();
					uint64_t id = remappedIDs[entDesc["ID"].get<uint64_t>()];
					uint64_t parent = entDesc["Parent"].is_null() ? entity::nullID : remappedIDs[entDesc["Parent"].get<uint64_t>()];
					transform trans = util::loadTransform(entDesc["Transform"]);

					std::vector<eleInitBase> elements;
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

			inline eleInitBase remapEleInitIDs(eleInitBase info, const std::map<uint64_t, uint64_t>& remappedIDs) {
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

			inline json savePrefabUnsafe(entity* toSave) {
				std::vector<entity*> connected = toSave->getAllConnected();
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