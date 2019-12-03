#pragma once

#include "elementRef.h"
#include "exceptions.h"

namespace citrus {
	template<class T>
	bool eleRef<T>::null() const {
		return !(_id && _id == _ptr->_ent->id);
	}

	template<class T>
	int64_t eleRef<T>::id() const {
		if (_id && (_id == abs(_ptr->_ent->id)))
			return _id;
		else
			return 0;
	}

	template<class T>
	T& eleRef<T>::get() const {
		if(null()) throw nullEleException("get(): null element");
		return *_ptr;
	}
	template<class T>
	T& eleRef<T>::operator*() const {
		return get();
	}
	template<class T>
	T* eleRef<T>::operator->() const {
		return &get();
	}

	template<class T>
	bool eleRef<T>::operator==(eleRef<T>* other) const {
		return _id == other._id;
	}
	template<class T>
	bool eleRef<T>::operator!=(eleRef<T>* other) const {
		return !(*this == other);
	}
	template<class T>
	eleRef<T>::operator bool() const {
		return !null();
	}

	template<class T>
	eleRef<T>& eleRef<T>::operator=(const eleRef<T>& other) {
		_ptr = other._ptr;
		_id = other._id;
		return *this;
	}

	template<class T>
	eleRef<T>::eleRef() : _ptr(nullptr), _id(0) { }
	template<class T>
	eleRef<T>::eleRef(T* ele) : _ptr(ele), _id(ele ? ele->_ent->id : 0) { }
}
