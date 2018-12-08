#pragma once

#include <engine/elementRef.h>
#include <engine/entityRef.h>

namespace citrus::engine {

	template<class T>
	bool eleRef<T>::null() const {
		return _ptr == nullptr || (!_owner.valid() && !_owner.null());
	}	

	template<class T>
	bool eleRef<T>::valid() const {
		return !null() && _owner.valid();
	}

	template<class T>
	T& eleRef<T>::get() const {
		if(!valid()) {
			if(_ptr == nullptr) {
				throw eleDereferenceException("eleRef is null");
			} else if(!_owner.null()) {
				throw eleDereferenceException("eleRef owner no longer exists");
			} else if(!_owner.initialized()) {
				throw eleDereferenceException("eleRef owner is not initialized");
			}
		}
		return *_ptr;
	}
	template<class T>
	T& eleRef<T>::operator*() const {
		return get();
	}
	
	template<class T>
	bool eleRef<T>::operator==(const eleRef<T>& other) const {
		return _ptr == other._ptr;
	}
	template<class T>
	bool eleRef<T>::operator==(std::nullptr_t np) const {
		return null();
	}
	template<class T>
	eleRef<T>::operator bool() const {
		return !null();
	}

	template<class T>
	eleRef<T>& eleRef<T>::operator=(const eleRef<T>& other) {
		_ptr = other._ptr;
		_owner = other._owner;
		return *this;
	}
	template<class T>
	eleRef<T>& eleRef<T>::operator=(std::nullptr_t np) {
		_ptr = NULL;
		_owner = entityRef();
		return *this;
	}

	template<class T>
	eleRef<T>::eleRef() : _owner(), _ptr(nullptr) { }
	template<class T>
	eleRef<T>::eleRef(T* ele) : _owner(ele->ent), _ptr(ele) { }
}