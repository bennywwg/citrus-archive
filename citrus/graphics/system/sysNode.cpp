#include "citrus/graphics/system/sysNode.h"
#include "citrus/graphics/system/renderSystem.h"

namespace citrus::graphics {
	void sysNode::addDependency(sysNode* pass) {
		dependencies.push_back(pass);
		pass->signalSems.push_back(sys.inst.createSemaphore());
		waitSems.push_back(pass->signalSems.back());
	}
	vector<sysNode*> sysNode::getDependencies() {
		return dependencies;
	}
	vector<VkSemaphore> sysNode::getWaitSems() {
		return waitSems;
	}
	vector<VkSemaphore> sysNode::getSignalSems() {
		return signalSems;
	}
	sysNode::sysNode(system& sys) : sys(sys) { }
	sysNode::~sysNode() {
		for (VkSemaphore const& sem : signalSems) {
			sys.inst.destroySemaphore(sem);
		}
	}
}