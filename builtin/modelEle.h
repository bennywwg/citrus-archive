#pragma once

#include "../mankern/element.h"
#include "../graphkern/mesh.h"
#include "../graphkern/meshPass.h"

namespace citrus {
	struct modelEleStruct {
		renderSystem* sys;
		double *time;
	};

	class modelEle : public element {
	public:
		renderSystem &sys;
		double &time;

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

		void action();

		void deserialize(citrus::json const& parsed);
		citrus::json serialize();

		modelEle(entRef const& ent, manager & man, void* usr);
		~modelEle();
	};
}
