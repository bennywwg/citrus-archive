#pragma once

#include "citrus/engine/entityRef.h"
#include "citrus/util.h"

#include "citrus/editor/gui.h"
#include "citrus/graphics/camera.h"
#include "citrus/graphics/system/immediatePass.h"
#include <memory>

#include <sol/sol.hpp>

namespace citrus {
	namespace engine {
		class renderManager;
		class camera;
		class manager;
		class engine;
	}
	
	namespace editor {
		using ::citrus::engine::entityRef;
		using ::citrus::engine::camera;

		class ctEditor {
			public:
			sol::state lua;

			vector<view> currentViews;
			shared_ptr<horiBar> topBar;
			vector<shared_ptr<floatingGui>> floating;

			entityRef hovered;
			entityRef selected;
			 
			ivec2 cursorPx;
			vec2 cursor;
			
			bool dragged = false;
			vec2 startDrag;
			ivec2 startDragPx;
			weak_ptr<floatingGui> draggedGui;
			ivec2 draggedGuiStart;

			// entity transformation stuff
			vec2 transDir;
			map<uint16_t, mat4> transMap;
			vec3 localTransDir;
			vec3 entityStartLocal;
			bool draggingEntity = false;

			bool playing = false;
			bool doFrame = false;


			engine::engine* eng;
			
			view* getHoveredView(ivec2 cursor);
			weak_ptr<floatingGui> getHoveredFloating(ivec2 cursor);
			void clearFloating(view* hovered);
			void renderAllGui();
			void mouseDown(uint16_t const& selectedIndex);
			void mouseUp(uint16_t const& selectedIndex);

			void update(graphics::immediatePass& ipass, uint16_t const& selectedIndex);
			void render(graphics::immediatePass & ipass);

			vector<string> toExec;
			std::mutex toExecMut;
			void setupShell();
			void shell(std::string const& ex);

			ctEditor();
		};
	}
}
