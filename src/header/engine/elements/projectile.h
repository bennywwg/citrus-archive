#include <engine/element.h>
#include <engine/engine.h>

namespace citrus::engine {
	class projectile : public element {
		public:
		float startTime, maxTime;
		glm::vec3 velocity;

		void preRender();

		projectile(entityRef ent);
	};
}