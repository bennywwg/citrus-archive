#pragma once

#include <engine/elementRef.h>
#include <memory>
#include <vector>
#include <glm/ext.hpp>
#include <dynamics/transform.h>
#include <mutex>
#include <editor/gui.h>

namespace citrus::engine {

	class entity;
	class engine;

	class entDereferenceException {
		public:
		std::string er;
		inline entDereferenceException(const std::string& er) : er(er) { }
	};

	class entityRef {
		friend class manager;
		friend class engine;

		engine* _eng;
		std::weak_ptr<entity> _ref;

		public:

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

		constexpr static uint64_t nullID = std::numeric_limits<uint64_t>::max();

		entityRef();

		private:

		entityRef(std::weak_ptr<entity> ref);
	};

}