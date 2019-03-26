#pragma once

#include "citrus/engine/entityRef.h"
#include "citrus/util.h"

#include "citrus/editor/gui.h"
#include <memory>

namespace citrus {
	namespace engine {
		class renderManager;
		class camera;
	}

	
	namespace editor {
		using ::citrus::engine::entityRef;
		using ::citrus::engine::renderManager;
		using ::citrus::engine::camera;

		struct guiFloating {
			ivec2 pos;
			float depth;
			bool persistent; //whether or not this floating GUI should continue existing even if it is clicked away from
			unique_ptr<gui> ele;
		};

		class ctEditor {
			void createTopBar();
			void createFileButton();
			void createEditButton();
			void createToolsButton();

			public:
			vector<view> currentViews;
			unique_ptr<gui> topBar;
			vector<guiFloating> floating;

			entityRef selected;
			
			bool dragged = false;
			vec2 startDrag;
			ivec2 startDragPx;

			bool playing = false;
			bool doFrame = false;


			bool translationMode;
			bool localTranslation;
			
			bool rotationMode;
			bool localRotation;

			engine::engine* eng;

			void click(ivec2 cursor);

			


			void update(renderManager& man, camera& cam);
			void render(renderManager& man, camera& cam);

			ctEditor();
		};
	}
}
