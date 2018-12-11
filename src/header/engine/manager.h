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
#include <engine/elementRef.h>
#include <engine/entityRef.h>

namespace citrus {
	namespace engine {
		using std::map;
		using std::vector;
		using std::function;
		using std::tuple;
		using std::pair;
		using std::string;
		using std::type_info;
		using std::type_index;
		using std::shared_ptr;

		class manager;
		class engine;

		class eleInitBase {
			public:
			const type_index type;
			const json data;
			const std::function<void(element*)> init;
			public:
			eleInitBase(type_index type, json data) : type(type), data(data) { }
			eleInitBase(type_index type, std::function<void(element*)> init) : type(type), data(json()), init(init) { }
			eleInitBase(type_index type, json data, std::function<void(element*)> init) : type(type), data(data), init(init) { }
		};
		template<class T> class eleInit : public eleInitBase {
			public:
			static eleInit<T> run(std::function<void(T&)> init) {
				return eleInit(init);
			}
			eleInit(json data) : eleInitBase(typeid(T), data) { }
			eleInit(std::function<void(T&)> init) : eleInitBase(typeid(T), std::function<void(element*)>([=](element* ele) { init(*((T*)(ele))); })) { }
			eleInit(json data, std::function<void(T&)> init) : eleInitBase(typeid(T), data, [=](element* ele) { init(*((T*)(ele))); }) { }
		};

		class manager {;
			friend class entity;
			friend class engine;
			struct elementInfo {
				function<void(element*, entityRef)> ctor;
				function<void(element*)> dtor;
				vector<tuple<element*, entityRef, json, std::function<void(element*)>>> toCreate;
				vector<pair<element*, entityRef>> toDestroy;
				vector<element*> existing;
				vector<entityRef> existingEntities;
				int size;
				string name;
				type_index type;
				std::shared_mutex mut;

				int findInToCreate(element* ele) {
					for(uint32_t i = 0; i < toCreate.size(); ++i)
						if(std::get<0>(toCreate[i]) == ele)
							return i;

					return -1;
				}
				int findInToDestroy(element* ele) {
					for(uint32_t i = 0; i < toDestroy.size(); ++i)
						if(toDestroy[i].first == ele)
							return i;

					return -1;
				}
				int findInExisting(element* ele) {
					for(uint32_t i = 0; i < existing.size(); ++i)
						if(existing[i] == ele)
							return i;

					return -1;
				}
				int findInExistingEntities(entityRef ent) {
					for(uint32_t i = 0; i < existingEntities.size(); ++i)
						if(existingEntities[i] == ent)
							return i;

					return -1;
				}

				elementInfo() : type(typeid(void)) { }
			};

			
	private: //stateless functions
			static int					     findEntity(const vector<shared_ptr<entity>>& entities, const shared_ptr<entity>& ent);
			static int					   containsType(const vector<eleInitBase>& infos, const type_index& type);
			static bool					   containsType(const vector<type_index>& types, const type_index& type);
			

			//manager state
			engine* const _eng;
			vector<type_index> _order;
			map<type_index, elementInfo> _data;
			private: vector<shared_ptr<entity>> _toCreate, _toDestroy, _entities;
			public: std::recursive_mutex toCreateMut, toDestroyMut, entitiesMut; //must lock in this order


			
	private: //element meta information
			elementInfo*					    getInfo(const type_index& index);
			elementInfo*				  getInfoByName(const string& name);
			vector<eleInitBase>					reorder(const vector<eleInitBase>& infos);
			vector<type_index>					reorder(const vector<type_index>& types);


	public: //element functions
			template<class T> void		   registerType(string name);
			void							   setOrder(const vector<type_index>& order);


	public: //creation and querying
			entityRef							 create(string name, const vector<eleInitBase>& data, uint64_t id);
			entityRef					   createUnsafe(string name, const vector<eleInitBase>& data, uint64_t id);
			void								destroy(entityRef ent);
			void						  destroyUnsafe(entityRef ent);
			vector<entityRef>				allEntities();
			entityRef						   findByID(uint64_t uuid);
			template<class T> vector<eleRef<T>>	 ofType();
			vector<element*>					 ofType(const type_index& index);


	public: //serialization
			json					    referenceEntity(entityRef ent);
			template<class T> json	   referenceElement(entityRef ent);
			entityRef				  dereferenceEntity(const json& data);
		template<class T> eleRef<T>	 dereferenceElement(const json& data);
			bool					  isEntityReference(const json& data);
			bool					 isElementReference(const json& data);
			eleInitBase					remapEleInitIDs(eleInitBase info, const std::map<uint64_t, uint64_t>& remappedIDs);
			void					   loadPrefabUnsafe(const json& data);
			void							 loadPrefab(const json& data);
			json					   savePrefabUnsafe(entityRef toSave);
			json							 savePrefab(entityRef toSave);

	private: //runtime
			void								  flush();
			void								 render();
			void							  preRender();
			void				    flushToCreateUnsafe();
			void				   flushToDestroyUnsafe();
			

			public:
			manager(engine* eng);
			~manager();

			private:
			manager(const manager& other) = delete;
			manager& operator=(const manager& other) = delete;
		};
	}
}
