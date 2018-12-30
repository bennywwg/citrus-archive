#pragma once

#include <engine/element.h>
#include <engine/engine.h>
#include <engine/elements/freeCam.h>
#include <engine/elements/rigidBodyComponent.h>
#include <engine/elements/meshFilter.h>

namespace citrus::engine {
	using namespace graphics::windowInput;

	class playerController : public element {
	public:
		float minDist = 2.0f, maxDist = 160.0f;
		float minX = 45.0f, maxX = 45.0f;
		float y = 45.0f, x = 30.0f, dist = 5.0f;
		float ySpeed = 70.0f, xSpeed = 90.0f, distSpeed = 5.0f;

		bool fired = false;
		bool walking = false;

		eleRef<freeCam> cam;
		eleRef<meshFilter> m;

		float navSpeed = 2.0f;

		void cameraStuff();
		void movementStuff();
		void actionStuff();
		void preRender();
		
		playerController(entityRef ent);
	};
}