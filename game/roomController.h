#pragma once

#include "../mankern/element.h"
#include "../mankern/elementRef.h"
#include "../builtin/freeCam.h"
#include "../builtin/modelEle.h"
#include "../builtin/sensorEle.h"
#include "../builtin/collisionEle.h"

namespace citrus {
	class roomController : public element {
		struct roomLinkage {
			string name;

			string connectedLevelTree;
			string connectedName;

			entRef loaded;

			void deserialize(json const& l);
			json serialize() const;
		};

		vector<roomLinkage> linkages;

		int findLinkageIndex(string const& name);

		entRef loadLinkage(int const& index);

		void unloadLinkage(entRef const& er);

		void deserialize(json const& js);
		json serialize();

		roomController(entRef const& ent, manager& man, void* usr);
	};
}
