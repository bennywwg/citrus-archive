#include <engine/manager.h>

namespace citrus::engine {
	template<class T> void manager::registerType(string name) {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		const auto& index = type_index(typeid(T));
		auto it = _data.find(index);
		if(it == _data.end()) {
			elementInfo& info = _data[index];
			info.ctor = function<void(element*, entityRef)>([](element* loc, entityRef ent) {
				new(loc) T(ent);
			});
			info.dtor = function<void(element*)>([](element* loc) { ((T*)loc)->~T(); });
			info.size = sizeof(T);
			info.type = index;
			info.name = name;
		}
	}

	template<class T> vector<eleRef<T>> manager::ofType() {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		vector<element*> pre = ofType(type_index(typeid(T)));
		vector<eleRef<T>> res;
		res.reserve(pre.size());
		for(element* ptr : pre) {
			res.push_back(eleRef<T>(dynamic_cast<T*>(ptr)));
		}
		return res;
	}

	template<class T> eleRef<T> manager::dereferenceElement(const json& data) {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		if(!isElementReference(data)) throw std::runtime_error(("Tried to derefence invalid element reference\n" + data.dump(2)).c_str());
		auto info = getInfo(typeid(T));
		if(data["Name"] != info->name) throw std::runtime_error(("Tried to derefence element but the template type does not match the json\n" + data.dump(2)).c_str());
		auto ent = findByID(data["ID"].get<uint64_t>());
		if(ent == nullptr) throw std::runtime_error("Tried to dereference element but its entity does not exist");
		//if(!ent.initialized()) throw std::runtime_error("Tried to dereference element but its entity is not initialized");
		return ent.getElement<T>();
	}

	template<class T> json manager::referenceElement(entityRef ent) {
		static_assert(std::is_base_of<element, T>::value, "T must be derived from element");
		auto info = getInfo(typeid(T));
		if(info == nullptr) throw std::runtime_error("Trying to reference element type that isn't registered");
		return json({
			{"Type", "Element Reference"},
			{"Name", info->name},
			{"ID", ent != nullptr ? ent.id() : entity::nullID}
			});
	}
}