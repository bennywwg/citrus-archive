#pragma once

#include <vulkan/vulkan.h>
#include "citrus/util.h"
#include "citrus/graphics/window.h"

namespace citrus::graphics {
	class system;

	class sysNode {
		friend class window;

	private:
		vector<sysNode*>	dependencies;

		// semaphore that is signalled upon completion of rendering
		VkSemaphore	const	signalSem;

	public:
		system& sys;

		// please use safely
		void				addDependency(sysNode* pass);
		vector<sysNode*>	getDependencies();
		vector<VkSemaphore>	getWaitSems();
		VkSemaphore			getSignalSem();

		// prepare for rendering proper, ie assign each thread item range
		// return value: number of invocations of renderPartial
		virtual void		preRender(uint32_t const& numThreads) = 0;

		// render for a certain thread, invoked multiple times concurrently
		virtual void		renderPartial(uint32_t const& threadIndex) = 0;

		// after all renderPartials complete
		virtual void		postRender(uint32_t const& numThreads) = 0;

		// manages semaphore lifetime
							sysNode(system& sys);
		virtual				~sysNode();
	};
}