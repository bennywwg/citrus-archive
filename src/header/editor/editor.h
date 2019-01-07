#pragma once

#include <engine/entityRef.h>
#include <util/glmUtil.h>

#include <editor/gui.h>

namespace citrus {
	namespace engine {
		class renderManager;
		class camera;
	}

	
	namespace editor {
		using ::citrus::engine::entityRef;
		using ::citrus::engine::renderManager;
		using ::citrus::engine::camera;

		class ctEditor {
			public:
			unique_ptr<gui> topBar;

			entityRef selected;
			
			bool dragged;
			vec2 startDrag;
			ivec2 startDragPx;

			bool translationMode;
			bool localTranslation;
			
			bool rotationMode;
			bool localRotation;


			void update(renderManager& man, camera& cam);
			void render(renderManager& man, camera& cam);

			ctEditor();
		};
	}
}