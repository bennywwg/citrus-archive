#include "citrus/graphics/system/sysNode.h"
#include "citrus/graphics/system/renderSystem.h"

namespace citrus::graphics {
	void sysNode::addDependency(sysNode* pass) {
		dependencies.push_back(pass);
	}
	vector<sysNode*> sysNode::getDependencies() {
		return dependencies;
	}
	vector<VkSemaphore> sysNode::getWaitSems() {
		vector<VkSemaphore> res;
		for (sysNode* dep : dependencies)
			res.push_back(dep->getSignalSem());
		return res;
	}
	VkSemaphore sysNode::getSignalSem() {
		return signalSem;
	}
	sysNode::sysNode(system& sys) : sys(sys), signalSem(sys.inst.createSemaphore()) { }
	sysNode::~sysNode() { sys.inst.destroySemaphore(signalSem); }
}