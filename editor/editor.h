#pragma once

#include "../mankern/entityRef.h"
#include "../mankern/util.h"

#include "gui.h"
#include "../graphkern/camera.h"
#include "../graphkern/immediatePass.h"
#include <memory>

//#include <sol/sol.hpp>

namespace citrus {
	class ctEditor {
		public:
		//sol::state lua;

		vector<view> currentViews;
		shared_ptr<horiBar> topBar;
		vector<shared_ptr<floatingGui>> floating;

		entRef hovered;
		entRef selected;
		std::map<int64_t, bool> expanded;

		float rx = 0.0f;
		float ry = 0.0f;
		camera cam;
			 
		ivec2 cursorPx;
		vec2 cursor;
			
		bool dragged = false;
		vec2 startDrag;
		ivec2 startDragPx;
		weak_ptr<floatingGui> draggedGui;
		ivec2 draggedGuiStart;

		// entity transformation stuff
		vec2 transDir;
		std::map<uint16_t, mat4> transMap;
		vec3 localTransDir;
		vec3 entityStartLocal;
		bool draggingEntity = false;

		bool playing = false;
		bool doFrame = false;

		manager *man;
		renderSystem* sys;
		window *win;

		view* getHoveredView(ivec2 cursor);
		weak_ptr<floatingGui> getHoveredFloating(ivec2 cursor);
		void clearFloating(view* hovered);
		void renderAllGui();
		void mouseDown(uint16_t const& selectedIndex);
		void mouseUp(uint16_t const& selectedIndex);

		void updateCam();
		void update(immediatePass& ipass, uint16_t const& selectedIndex);
		void render(immediatePass & ipass);

		void addToScenceHierarchy(weak_ptr<floatingContainer> weakRef, entRef ent, int level);
		void renderSceneHierarchy(weak_ptr<floatingContainer> weakRef);
		void renderEntityInspector( weak_ptr<floatingContainer> weakRef);

		vector<string> toExec;
		std::mutex toExecMut;
		//void setupShell();
		//void shell(std::string const& ex);

		ctEditor(manager *man, renderSystem *sys, window *win);
	};
}
