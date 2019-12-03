#pragma once

#include "util.h"
#include "window.h"

namespace citrus {
	class renderSystem;

	class sysNode {
		friend class window;

	private:
		std::vector<sysNode*>	dependencies; // semaphore is needed
		
		std::vector<VkSemaphore> waitSems;
		std::vector<VkSemaphore> signalSems; // managed by this object

	public:
		renderSystem& sys;

		// please use safely
		void				addDependency(sysNode* pass);
		std::vector<sysNode*>	getDependencies();
		std::vector<VkSemaphore>	getWaitSems();
		std::vector<VkSemaphore>	getSignalSems();

		// prepare for rendering proper, ie assign each thread item range
		// return value: number of invocations of renderPartial
		virtual void		preRender(uint32_t const& numThreads) = 0;

		// render for a certain thread, invoked multiple times concurrently
		virtual void		renderPartial(uint32_t const& threadIndex) = 0;

		// after all renderPartials complete
		virtual void		postRender(uint32_t const& numThreads) = 0;

		// manages semaphore lifetime
							sysNode(renderSystem& sys);
		virtual				~sysNode();
	};
}