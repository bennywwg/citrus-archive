#pragma once

#include "renderSystem.h"
#include "runtimeResource.h"
#include "sysNode.h"
#include "util.h"
#include "../mankern/editorGrouping.h"

namespace citrus {
	class immediatePass : public sysNode {
		uint64_t const			uboSize = 1024 * 1024;
		uint64_t const			vertSize = 128 * 1024 * 1024;
	protected:
		fpath vert, frag;

		VkDescriptorSetLayout	uboLayout, texLayout;
		VkDescriptorPool		uboPool, texPool;
		VkDescriptorSet			uboSets[SWAP_FRAMES];
		VkDescriptorSet			texSet;

		VkRenderPass			pass;
		VkPipelineLayout		pipelineLayout;
		VkPipeline				pipeline;
		VkCommandBuffer			priBufs[SWAP_FRAMES];
		VkFence					waitFences[SWAP_FRAMES];

		buffer					ubos[SWAP_FRAMES];
		buffer					verts[SWAP_FRAMES];

		bool					transitionToRead;

	public:
		frameStore* const	frame;
	protected:
		VkFramebuffer		fbos[SWAP_FRAMES];

		virtual void			initializeDescriptors();
		virtual void			initializeRenderPass();
		virtual void			initializePipelineLayout();
		virtual void			initializePipeline();
		virtual void			initializeFramebuffers();
	public:

		// these get bitwised OR'd with grouping index in groupings
		uint16_t indexBits;

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
		virtual void            postRender(uint32_t const& threadCount);

		struct uniformBlock {
			mat4 mvp;
			vec4 color;
			uint32_t index;
		};

		std::vector<grouping>	groupings;

		bool				active = true;
		bool				wireframe = false;

		immediatePass(renderSystem& sys, frameStore* fstore, fpath const& vert, fpath const& frag, bool wireframe, bool transitionToRead);
		~immediatePass();
	};
}