#pragma once

#include "citrus/util.h"
#include "citrus/editor/gui.h"

namespace citrus::engine {

	class entity;
	class engine;
	template<class T> class eleRef;

	class entDereferenceException {
		public:
		std::string er;
		inline entDereferenceException(const std::string& er) : er(er) { }
	};

	class entityRef {
		friend class manager;
		friend class engine;

		engine* _eng;
		weak_ptr<entity> _ref;

		public:
		
		weak_ptr<entity> raw() const;
		weak_ptr<entity> operator*() const;

		std::unique_ptr<editor::gui> renderGUI() const;

		bool null() const;

		uint64_t id() const;
		std::string name() const;
		engine* eng() const;

		void setLocalTransform(const transform &trans) const;
		void setLocalPosition(const glm::vec3& pos) const;
		void setLocalOrientation(const glm::quat& ori) const;
		void setLocalScale(const glm::vec3& scale) const;
		transform getLocalTransform() const;
		glm::vec3 getLocalPosition() const;
		glm::quat getLocalOrientation() const;
		transform getGlobalTransform() const;

		void setParent(entityRef parent) const;
		entityRef getRoot() const;
		entityRef getParent() const;
		std::vector<entityRef> getChildren() const;
		std::vector<entityRef> getAllConnected() const;

		template<class T>
		eleRef<T> getElement();

		bool operator==(const entityRef& other) const;
		bool operator==(const std::nullptr_t& null) const;
		bool operator!=(const entityRef& other) const;
		bool operator!=(const std::nullptr_t& null) const;
		operator bool() const;

		constexpr static uint64_t nullID = 0;

		entityRef();

		private:

		entityRef(std::weak_ptr<entity> ref);
	};

}
