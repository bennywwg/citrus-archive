#include "roomController.h"

namespace citrus {
	int roomController::findLinkageIndex(string const& name) {
		for (int i = 0; i < linkages.size(); i++) {
			if (linkages[i].name == name) return i;
		}
		return -1;
	}
	entRef roomController::loadLinkage(int const& index) {
		if (linkages[index].loaded) return linkages[index].loaded;
		try {
			entRef res = man().loadTree(linkages[index].connectedLevelTree);
			mat4 outer = ent().getLocalTrans().getMat();
			outer = outer * linkages[index].connectedRoomTrans.getMat();
			res.setLocalTrans(outer);
		} catch (invalidPrefabException const& ex) {
			std::cout << "failed to load level: " << ex.message << "\n";
			return nullptr;
		}
	}
	void roomController::unloadLinkage(entRef const& er) {
		for (int i = 0; i < linkages.size(); i++) {
			if (linkages[i].loaded == er) {
				man().destroy(er);
				break;
			}
		}
	}
	void roomController::deserialize(json const& j) {
		if (j.is_array()) {
			for (json l : j) {
				roomLinkage rl;
				rl.deserialize(l);
				linkages.push_back(rl);
			}
		}
	}
	json roomController::serialize() {
		json::array_t res;
		for (roomLinkage const& rl : linkages) {
			res.push_back(rl.serialize());
		}
		return res;
	}
	roomController::roomController(entRef const& ent, manager& man, void* usr) : element(ent, man, usr, typeid(roomController)) {
	}

	void roomController::roomLinkage::deserialize(json const& l) {
		name = l["name"].get<string>();
		myTrans = loadTransform(l["trans"]);
		connectedLevelTree = l["connectedLevelTree"].get<string>();
		connectedLinkageName = l["connectedLinkageName"].get<string>();
		connectedRoomTrans = loadTransform(l["connectedRoomTrans"]);
	}
	json roomController::roomLinkage::serialize() const {
		json l;
		l["name"] = name;
		l["trans"] = citrus::save(myTrans);
		l["connectedLevelTree"] = connectedLevelTree;
		l["connectedLinkageName"] = connectedLinkageName;
		l["connectedRoomTrans"] = citrus::save(connectedRoomTrans);
		return l;
	}
}