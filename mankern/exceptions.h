#pragma once

#include <string>

#define DECLARE_MAN_EXCEPTION(classname, parentClass) \
class classname : public parentClass { \
public: \
	classname(std::string const& msg) : parentClass(msg) { } \
};

namespace citrus {
	class managerException {
	public:
		std::string message;
		managerException(std::string const& msg) : message(msg) { }
	};

	DECLARE_MAN_EXCEPTION(nullEntityException, managerException) // tried to perform operation on null entity
	DECLARE_MAN_EXCEPTION(nullEleException, managerException) // tried to perform operation on null element
	DECLARE_MAN_EXCEPTION(invalidPrefabException, managerException) // anything went wrong in prefab
	DECLARE_MAN_EXCEPTION(unknownElementException, invalidPrefabException) // anything went wrong in prefab
	// prefab hierarchy is invalid (most likely a reference to ent / ele outside the hierarchy was found)
	DECLARE_MAN_EXCEPTION(invalidPrefabHierarchyException, invalidPrefabException)

	/*class nullEntityException : public managerException {
	public:
		nullEntityException(std::string const& msg) : managerException(msg) { }
	};

	class nullEleException : public managerException {
	public:
		nullEleException(std::string const& msg) : managerException(msg) { }
	};

	class invalidPrefabException : public managerException {
	public:
		invalidPrefabException(std::string const& msg) : managerException(msg) { }
	};*/
}