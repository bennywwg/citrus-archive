#include "sysNode.h"
#include "renderSystem.h"

namespace citrus {
	void sysNode::addDependency(sysNode* pass) {
		dependencies.push_back(pass);
		pass->signalSems.push_back(sys.inst.createSemaphore());
		waitSems.push_back(pass->signalSems.back());
	}
	std::vector<sysNode*> sysNode::getDependencies() {
		return dependencies;
	}
	std::vector<VkSemaphore> sysNode::getWaitSems() {
		return waitSems;
	}
	std::vector<VkSemaphore> sysNode::getSignalSems() {
		return signalSems;
	}
	sysNode::sysNode(renderSystem& sys) : sys(sys) { }
	sysNode::~sysNode() {
		for (VkSemaphore const& sem : signalSems) {
			sys.inst.destroySemaphore(sem);
		}
	}
}