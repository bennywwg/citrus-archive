#pragma once

#include "citrus/engine/element.h"
#include "citrus/engine/engine.h"
#include "citrus/graphics/mesh.h"

namespace citrus::engine {
	class meshFilter : public element {
	private:
		int _model = -1;
		int _tex = -1;
		int _ani = -1;
		graphics::behavior _mode;
		double _aniStart = 0.0;

	public:
		glm::vec3 color;
		bool visible = true;;
		int systemIndex = -1;

		void setState(int m, int t, int a = -1);
		int model() const;
		int tex() const;
		int ani() const;
		graphics::behavior mode() const;
		double aniTime() const;
		void startAnimation(int ani, graphics::behavior mode = graphics::behavior::nearest);

		std::unique_ptr<editor::gui> renderGUI();

		void load(const json&);

		meshFilter(entityRef ent);
		~meshFilter();
	};
}
