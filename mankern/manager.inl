#include "manager.h"
#include "exceptions.h"

//#define IGNORE_DUPLICATE_OP

namespace citrus {
	template<class T> void manager::registerType(string name, bool active, void *usrPtr) {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		elementInfo* info = new elementInfo(type_index(typeid(T)), this);
		info->name = name;
		manager* man = info->parentManager;
		info->ctor =
			function<void(element*, entity*)>([man, usrPtr](element* loc, entity* ent) { new(loc) T(entRef(ent), *man, usrPtr); });
		info->dtor =
			function<void(element*)>([](element* loc) { ((T*)loc)->~T(); });
		info->size = sizeof(T);
		info->active = active;
		info->usrPtr = usrPtr;
		info->elinit();
		_data.emplace_back(type_index(typeid(T)), info);
	}

	template<class T>
	std::vector<eleRef<T>> manager::ofType() {
		std::vector<eleRef<T>> res;
		auto inf = getInfo(typeid(T));
		for (T* cur = (T*)inf->allocBegin; cur; cur = (T*)cur->next) {
			res.push_back(eleRef<T>(cur));
		}
		return res;
	}

	template<typename T>
	inline eleRef<T> citrus::manager::addElement(entRef ent) {
		if (!ent) throw nullEntityException("addElement() : invalid entity");
		auto const& t = type_index(typeid(T));
		for (auto const& e : ent._ptr->eles) if (e->_type == t)
#ifndef IGNORE_DUPLICATE_OP
			throw std::runtime_error("duplicate entity error");
#else
			return;
#endif
		elementInfo* inf = getInfo(t);
		T* res = (T*)inf->elalloc();
		((element*)res)->_ent = ent._ptr;
		inf->toCreate.emplace_back(res);
		return eleRef<T>(res);
	}

	template<typename T>
	inline eleRef<T> citrus::manager::addElement(entRef ent, vector<uint8_t> const& binData) {
		if (!ent) throw nullEntityException("addElement() : invalid entity");
		auto const& t = type_index(typeid(T));
		for (auto const& e : ent._ptr->eles) if (e->_type == t)
#ifndef IGNORE_DUPLICATE_OP
			throw std::runtime_error("duplicate entity error");
#else
			return;
#endif
		elementInfo* inf = getInfo(t);
		T* res = (T*)inf->elalloc();
		((element*)res)->_ent = ent._ptr;
		inf->toCreate.emplace_back(res, binData);
		return eleRef<T>(res);
	}

	template<typename T>
	inline eleRef<T> citrus::manager::addElement(entRef ent, json const& j) {
		if (!ent) throw nullEntityException("addElement() : invalid entity");
		auto const& t = type_index(typeid(T));
		for (auto const& e : ent._ptr->eles) if (e->_type == t)
#ifndef IGNORE_DUPLICATE_OP
			throw std::runtime_error("duplicate entity error");
#else
			return;
#endif
		elementInfo* inf = getInfo(t);
		T* res = (T*)inf->elalloc();
		((element*)res)->_ent = ent._ptr;
		inf->toCreate.emplace_back(res, j);
		return eleRef<T>(res);
	}

	template<typename T>
	inline void citrus::manager::destroyElement(eleRef<T> ele) {
		destroyElement(ele._ptr);
	}

	//template<class T> eleRef<T> manager::dereferenceElement(const json& data) {
	//	static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
	//	if(!isElementReference(data)) throw std::runtime_error(("Tried to derefence invalid element reference\n" + data.dump(2)).c_str());
	//	auto info = getInfo(typeid(T));
	//	if(data["Name"] != info->name) throw std::runtime_error(("Tried to derefence element but the template type does not match the json\n" + data.dump(2)).c_str());
	//	auto ent = findByID(data["ID"].get<uint64_t>());
	//	if(ent == nullptr) throw std::runtime_error("Tried to dereference element but its entity does not exist");
	//	//if(!ent.initialized()) throw std::runtime_error("Tried to dereference element but its entity is not initialized");
	//	return ent.getElement<T>();
	//}

	//template<class T> json manager::referenceElement(entRef ent) {
	//	static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
	//	auto info = getInfo(typeid(T));
	//	if(info == nullptr) throw std::runtime_error("Trying to reference element type that isn't registered");
	//	return json({
	//		{"Type", "Element Reference"},
	//		{"Name", info->name},
	//		{"ID", ent != nullptr ? ent.id() : entity::nullID}
	//	});
	//}
}
