#pragma once

#include <map>
#include <typeinfo>
#include <vector>
#include <typeindex>
#include <functional>
#include <shared_mutex>
#include <atomic>

#include "element.h"

#include "entity.h"

namespace citrus {
	namespace engine {
		class manager;
		class engine;
		

		class manager {
			engine* const _eng;

			struct elementInfo {
				std::function<void(element*, entity*)> ctor;
				std::function<void(element*)> dtor;
				std::vector<std::pair<const elementMeta*, entity*>> toCreate;
				std::vector<std::pair<const elementMeta*, entity*>> toDestroy;
				std::vector<element*> existing;
				std::vector<entity*> existingEntities;
				int size;
				std::type_index type;

				elementInfo() : type(typeid(void)) { }
			};
			std::map<std::type_index, elementInfo> _data;

			std::vector<std::type_index> _order;

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

			void checkType(const std::type_index& type) {
				for(unsigned int i = 0; i < _order.size(); i++)
					if(_order[i] == type)
						return;
				throw std::exception("manager type check failed");
			}

			//returns if a vector of types contains a certain type
			//O(types.size())
			bool containsType(const std::vector<std::type_index>& types, const std::type_index& type) {
				for(const auto& t : types)
					if(t == type)
						return true;
				return false;
			}

			int findEntity(const std::vector<entity*>& entities, entity* ent) {
				for(unsigned int i = 0; i < entities.size(); ++i)
					if(ent == entities[i])
						return i;
				
				return -1;
			}
			int findElementMeta(const std::vector<std::pair<const elementMeta*, entity*>>& elements, const elementMeta* ele) {
				for(unsigned int i = 0; i < elements.size(); ++i)
					if(ele == elements[i].first)
						return i;

				return -1;
			}

			//takes in a vector of type_indexes, and returns
			//a corresponding vector of elementToCreate, in the order
			//specified by _order and ignoring elements not in _order
			//it allocates memory with ::operator new(size) so be sure to ::operator delete
			//and it creates a new entity, be sure to delete that too
			//O(_order.size() * types.size())
			entity* newEntity(const std::vector<std::type_index>& types) {
				std::vector<elementMeta> res;

				for(const auto& oType : _order)
					if(containsType(types, oType)) {
						const auto& info = getInfo(oType);
						res.emplace_back(info->size, oType, (element*)(::operator new(info->size)));
					}

				return new entity(res, _eng);
			}

			//reorders a vector of types into the same order as _order
			//and eliminates elements that don't exist in _order
			//O(_order.size() * types.size())
			std::vector<std::type_index> reorder(const std::vector<std::type_index>& types) {
				std::vector<std::type_index> res;

				for(const auto& type : _order)
					if(containsType(types, type))
						res.push_back(type);

				return res;
			}

			public:
			//lock this to prevent your underlying objects from being deleted
			std::shared_mutex objectMut;

			//makes an element type usable
			template<class T> inline void registerType() {
				static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
				const auto& index = std::type_index(typeid(T));
				auto it = _data.find(index);
				if(it == _data.end()) {
					elementInfo& info = _data[index];
					info.ctor = std::function<void(element*, entity*)>([](element* loc, entity* ent) { new(loc) T(ent); });
					info.dtor = std::function<void(element*)>([](element* loc) { ((T*)loc)->~T(); });
					info.size = sizeof(T);
					info.type = index;
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
			inline entity* create(const std::vector<std::type_index>& types) {
				std::shared_lock<std::shared_mutex> lock(toCreateMut);
				return createUnsafe(types);
			}
			//queue an object for creation
			//this object is valid until destroy() is called for it flush() is called, at which point its memory is freed
			//!!! must hard lock toCreateMut !!!
			inline entity* createUnsafe(const std::vector<std::type_index>& types) {
				for(auto& type : types)
					checkType(type);

				auto ent = newEntity(types);
				for(auto& ele : ent->_elements)
					getInfo(ele._type)->toCreate.emplace_back(&ele, ent);

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
						auto& tc = getInfo(meta._type)->toCreate;

						int eleIndex = findElementMeta(tc, &meta);
						if(eleIndex != -1) {
							::operator delete(meta._ele);
							tc.erase(tc.begin() + eleIndex);
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
					getInfo(meta._type)->toDestroy.emplace_back(&meta, ent);
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
						info.ctor(meta.first->_ele, meta.second);
						info.existingEntities.push_back(meta.second);
						info.existing.push_back(meta.first->_ele);
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
						meta.first->_ele->onCreate();
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

				//call onDestroy for all elements in order
				for(const auto& oType : _order) {
					auto& info = *getInfo(oType);

					for(auto& meta : info.toDestroy) {
						meta.first->_ele->onDestroy();
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
							if(meta.first->_ele == info.existing[i])
								info.existing.erase(info.existing.begin() + i);


						info.dtor(meta.first->_ele);
						::operator delete(meta.first->_ele);
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