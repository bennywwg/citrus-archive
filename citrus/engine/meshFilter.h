#pragma once

#include "citrus/engine/element.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/mesh/mesh.h"
#include "citrus/graphics/system/meshPass.h"

namespace citrus::engine {
	class meshFilter : public element {
	private:
		int materialIndex = -1;
		int itemIndex = -1;
		graphics::behavior _mode;
		double _aniStart = 0.0;

	public:
		glm::vec3 color;
		bool visible = true;

		void setState(int matIndex, int modelIndex, int texIndex);
		
		void reset();
		int model() const;
		int tex() const;
		int ani() const;
		graphics::behavior mode() const;
		double aniTime() const;
		void startAnimation(int ani, graphics::behavior mode = graphics::behavior::nearest);

		std::unique_ptr<editor::gui> renderGUI();

		void preRender();

		void load(const json&);

		meshFilter(entityRef ent);
		~meshFilter();
	};
}
