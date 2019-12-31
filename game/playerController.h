#pragma once

#include "../mankern/element.h"
#include "../mankern/elementRef.h"
#include "../builtin/freeCam.h"
#include "../builtin/rigidEle.h"

namespace citrus {
	class playerController : public element {
	public:
		float minDist = 2.0f, maxDist = 160.0f;
		float minX = 45.0f, maxX = 45.0f;
		float y = 45.0f, x = 30.0f, dist = 5.0f;
		float ySpeed = 70.0f, xSpeed = 90.0f, distSpeed = 5.0f;

		bool fired = false;
		bool walking = false;

		window* const win;

		eleRef<freeCam> cam;

		eleRef<rigidEle> body;

		float navSpeed = 2.0f;

		void cameraStuff();
		void movementStuff();
		void actionStuff();
		void action();
		
		playerController(entRef const& ent, manager& man, void* usr);
	};
}
