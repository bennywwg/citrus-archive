#pragma once

#include <vector>
#include <memory>
#include <string>

#include "util.h"
#include "elementRef.inl"
#include "entity.h"

namespace citrus {
	class entity;
	class element;
	class ctEditor;

	class entRef {
		friend class manager;
		friend class element;
		friend class ctEditor;

		// 0 -> no pointed entity
		int64_t _id;
		entity* _ptr;

	public:
		// peer into the manager's soul
		entity* _raw() const;

		// returns false if the pointed entity is valid
		// returns true otherwise
		bool null() const;

		// returns the id of pointed entity if valid or pending
		// otherwise returns nullID
		int64_t id() const;

		void setName(std::string const& st) const;
		std::string name() const;

		entRef getParent() const;
		std::vector<entRef> getChildren() const;

		// local spatial function
		void setLocalPos(vec3 const& pos) const;
		void setLocalOri(quat const& ori) const;
		void setLocalTrans(transform const& trans) const;
		vec3 getLocalPos() const;
		quat getLocalOri() const;
		transform getLocalTrans() const;

		transform getGlobalTrans() const;

		template<class T>
		inline eleRef<T> getEle() const {
			std::type_index const& ti = typeid(T);
			for (int i = 0; i < _ptr->eles.size(); i++) {
				if (_ptr->eles[i]->_state._type == ti) {
					return eleRef<T>((T*)_ptr->eles[i]);
				}
			}
			return eleRef<T>();
		}

		bool operator==(entRef const&other) const;
		bool operator!=(entRef const& other) const;
		operator bool() const;

		entRef& operator=(const entRef& other);

		entRef();

	private:

		entRef(entity* e);
	};
}
