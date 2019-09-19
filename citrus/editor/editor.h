#pragma once

#include "citrus/engine/entityRef.h"
#include "citrus/util.h"

#include "citrus/editor/gui.h"
#include "citrus/graphics/camera.h"
#include "citrus/graphics/system/immediatePass.h"
#include <memory>

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

		struct guiFloating {
			ivec2 pos;
			float depth;
			bool justCreated = true;
			bool persistent; //whether or not this floating GUI should continue existing even if it is clicked away from
			shared_ptr<gui> ele;
		};

		class ctEditor {
			public:
			vector<view> currentViews;
			shared_ptr<gui> topBar;
			vector<shared_ptr<guiFloating>> floating;

			entityRef hovered;
			entityRef selected;
			
			bool dragged = false;
			vec2 startDrag;
			ivec2 startDragPx;
			weak_ptr<guiFloating> draggedGui;
			ivec2 draggedGuiStart;

			bool playing = false;
			bool doFrame = false;


			bool translationMode;
			bool localTranslation;
			
			bool rotationMode;
			bool localRotation;

			engine::engine* eng;
			
			view* getHoveredView(ivec2 cursor);
			weak_ptr<guiFloating> getHoveredFloating(ivec2 cursor);
			void clearFloating(view* hovered);
			void renderAllGui();
			void mouseDown(ivec2 cursor);
			void mouseUp(ivec2 cursor);

			void update(ivec2 cursor, uint16_t const& selectedIndex);
			void render(graphics::immediatePass & ipass);

			ctEditor();
		};
	}
}
