#pragma once

#include "renderSystem.h"
#include "sysNode.h"
#include "runtimeResource.h"

namespace citrus {
	/*class layoutTransition : public sysNode {
	protected:
		frameStore* const		frame;
		VkCommandBuffer			priBufs[SWAP_FRAMES];
		VkFence					waitFences[SWAP_FRAMES];
	public:

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
		virtual void            postRender(uint32_t const& threadCount);

		layoutTransition(renderSystem& sys, frameStore* fstore);
		~layoutTransition();
	};*/

	class clearFrame : public sysNode {
	protected:
		VkRenderPass			pass;
		VkCommandBuffer			priBufs[SWAP_FRAMES];

		frameStore* const		frame;
		VkFramebuffer			fbos[SWAP_FRAMES];
		VkFence					waitFences[SWAP_FRAMES];

		virtual void			initializeRenderPass();
		virtual void			initializeFramebuffers();

		int frameNum = 0;
	public:

		uint32_t				cursorX, cursorY;
		uint16_t				selectedIndex;

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
		virtual void            postRender(uint32_t const& threadCount);

		clearFrame(renderSystem& sys, frameStore* fstore);
		~clearFrame();
	};
}