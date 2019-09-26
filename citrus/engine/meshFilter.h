#pragma once

#include "citrus/engine/element.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/mesh/mesh.h"
#include "citrus/graphics/system/meshPass.h"

namespace citrus::engine {
	class meshFilter : public element {
	public:
		int materialIndex = -1;
		int itemIndex = -1;
		graphics::behavior _mode;
		double _aniStart = 0.0;

		glm::vec3 color;
		bool visible = true;

		void setState(int matIndex, int modelIndex, int texIndex);
		void setNormalMap(int texIndex);
		
		void reset();
		int model() const;
		int tex() const;
		int ani() const;
		graphics::behavior mode() const;
		float aniTime() const;
		void startAnimation(int ani, graphics::behavior mode = graphics::behavior::nearest);

		std::unique_ptr<editor::gui> renderGUI();

		void preRender();

		void load(const citrus::json&);
		citrus::json save();

		meshFilter(entityRef ent);
		~meshFilter();
	};
}
