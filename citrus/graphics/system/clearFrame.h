#pragma once

#include <citrus/graphics/system/renderSystem.h>
#include <citrus/graphics/system/sysNode.h>
#include <citrus/graphics/system/runtimeResource.h>

namespace citrus::graphics {
	/*class layoutTransition : public sysNode {
	protected:
		frameStore* const		frame;
		VkCommandBuffer			priBufs[SWAP_FRAMES];
		VkFence					waitFences[SWAP_FRAMES];
	public:

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
		virtual void            postRender(uint32_t const& threadCount);

		layoutTransition(system& sys, frameStore* fstore);
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
	public:

		uint16_t				selectedIndex;

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
		virtual void            postRender(uint32_t const& threadCount);

		clearFrame(system& sys, frameStore* fstore);
		~clearFrame();
	};
}