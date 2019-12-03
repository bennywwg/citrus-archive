#pragma once

#include "../mankern/element.h"
#include "../graphkern/mesh.h"
#include "../graphkern/meshPass.h"

namespace citrus {
	class modelEle : public element {
	public:
		int materialIndex = -1;
		int itemIndex = -1;
		behavior _mode;
		double _aniStart = 0.0;

		vec3 color;
		bool visible = true;

		void setState(int matIndex, int modelIndex, int texIndex);
		void setNormalMap(int texIndex);
		
		void reset();
		int model() const;
		int tex() const;
		int ani() const;
		behavior mode() const;
		float aniTime() const;
		void startAnimation(int ani, behavior mode = behavior::nearest);

		//std::unique_ptr<editor::gui> renderGUI();

		void preRender();

		void load(const citrus::json&);
		citrus::json save();

		string name() const;

		meshFilter(entityRef ent);
		~meshFilter();
	};
}
