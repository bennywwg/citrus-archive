#pragma once

#include <map>
#include <typeinfo>
#include <vector>
#include <typeindex>
#include <functional>
#include <shared_mutex>
#include <atomic>

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
			eleInitBase(std::type_index type, nlohmann::json data) : type(type), data(data) { }
		};
		template<class T>
		class eleInit : public eleInitBase {
			public:
			eleInit(nlohmann::json data) : eleInitBase(typeid(T), data) { }
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

				int findInToCreate(element* ele) {
					for(int i = 0; i < toCreate.size(); ++i)
						if(std::get<0>(toCreate[i]) == ele)
							return i;

					return -1;
				}
				int findInToDestroy(element* ele) {
					for(int i = 0; i < toDestroy.size(); ++i)
						if(toDestroy[i].first == ele)
							return i;

					return -1;
				}
				int findInExisting(element* ele) {
					for(int i = 0; i < existing.size(); ++i)
						if(existing[i] == ele)
							return i;

					return -1;
				}
				int findInExistingEntities(entity* ent) {
					for(int i = 0; i < existingEntities.size(); ++i)
						if(existingEntities[i] == ent)
							return i;

					return -1;
				}

				elementInfo() : type(typeid(void)) { }
			};
			std::map<std::type_index, elementInfo> _data;

			std::vector<std::type_index> _order;

			private:
			int findEntity(const std::vector<entity*>& entities, entity* ent) {
				for(int i = 0; i < entities.size(); ++i)
					if(entities[i] == ent)
						return i;

				return -1;
			}

			private: std::vector<entity*> _toCreate;
			public: std::shared_mutex toCreateMut;

			private: std::vector<entity*> _toDestroy;
			public: std::shared_mutex toDestroyMut;

			private: std::vector<entity*> _entities;
			public: std::shared_mutex entitiesMut;

			private:
			//gets the elementInfo for a type if it exists
			elementInfo* getInfo(const std::type_index& index) {
				auto it = _data.find(index);
				if(it != _data.end()) {
					return &(it->second);
				} else {
					return nullptr;
				}
			}

			bool containsType(const std::vector<std::type_index>& types, const std::type_index& type) {
				for(const auto& t : types)
					if(t == type)
						return true;
				return false;
			}
			int containsType(const std::vector<eleInitBase>& infos, const std::type_index& type) {
				for(int i = 0; i < infos.size(); i++) {
					const auto& info = infos[i];
					if(info.type == type)
						return i;
				}
					
				return -1;
			}

			std::vector<std::type_index> reorder(const std::vector<std::type_index>& types) {
				std::vector<std::type_index> res;

				for(const auto& type : _order)
					if(containsType(types, type))
						res.push_back(type);

				return res;
			}
			std::vector<eleInitBase> reorder(const std::vector<eleInitBase>& infos) {
				std::vector<eleInitBase> res;

				for(const auto& type : _order) {
					int index = containsType(infos, type);
					if(index != -1)
						res.push_back(infos[index]);
				}

				return res;
			}

			entity* newEntity(std::string name, const std::vector<eleInitBase>& orderedData, uint64_t id) {
				std::vector<elementMeta> types;
				types.reserve(orderedData.size());
				for(const auto& info : orderedData) {
					const auto& traits = getInfo(info.type);
					types.emplace_back(info.type, (element*)(::operator new(traits->size)));
				}

				return new entity(types, _eng, name, id);
			}

			public:
			//lock this to prevent your underlying objects from being deleted
			std::shared_mutex objectMut;

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
			inline entity* create(std::string name, const std::vector<eleInitBase>& data) {
				std::shared_lock<std::shared_mutex> lock(toCreateMut);
				return createUnsafe(name, data);
			}
			//queue an object for creation
			//this object is valid until destroy() is called for it flush() is called, at which point its memory is freed
			//!!! must hard lock toCreateMut !!!
			inline entity* createUnsafe(std::string name, const std::vector<eleInitBase>& data) {
				auto orderedData = reorder(data);

				auto ent = newEntity(name, orderedData, util::nextID());
				
				for(int i = 0; i < ent->_elements.size(); ++i)
					getInfo(ent->_elements[i].type)->toCreate.emplace_back(ent->_elements[i].ele, ent, orderedData[i].data);

				_toCreate.push_back(ent);

				return ent;
			}

			//queue an object for deletion
			//the object is still valid until flush() is called, at which point its memory is freed
			//!!! hard locks toCreateMut and toDestroMut !!!
			inline void destroy(entity* obj) {
				std::unique_lock<std::shared_mutex> lock0(toCreateMut), lock1(toDestroyMut);
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
				std::unique_lock<std::shared_mutex> lock(toCreateMut), lock1(toDestroyMut), lock2(entitiesMut);
				
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