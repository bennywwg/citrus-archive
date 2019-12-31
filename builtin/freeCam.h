#pragma once

#include "../mankern/element.h"
#include "../graphkern/renderSystem.h"

namespace citrus {
	class freeCam : public element {
	public:
		camera cam;

		bool enabled = true;

		void action();

		freeCam(entRef const& ent, manager& man, void* usr);
		~freeCam() = default;
	};
}