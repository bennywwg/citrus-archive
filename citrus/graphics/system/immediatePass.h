#pragma once

#include <citrus/graphics/system/renderSystem.h>
#include <citrus/graphics/system/runtimeResource.h>
#include <citrus/graphics/system/sysNode.h>

namespace citrus::graphics {
	class immediatePass : public sysNode {
		uint64_t const			uboSize = 4 * 1024;
		uint64_t const			vertSize = 4 * 1024 * 1024;
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

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
		virtual void            postRender(uint32_t const& threadCount);

		struct grouping {
			mat4			tr;
			vec3			color;
			vector<vec3>	data;
			vector<vec2>	uvdata;
			bool			pixelspace;
			/// add a mesh presenting a string to this grouping
			/// fills in any missing uv data if needed
			void addText(string text, int px, ivec2 pos = ivec2(0, 0));
		};

		struct uniformBlock {
			mat4 mvp;
			vec4 color;
			uint32_t index;
		};

		vector<grouping>	groupings;

		bool				active = true;

		immediatePass(system& sys, frameStore* fstore, fpath const& vert, fpath const& frag, bool transitionToRead);
		~immediatePass();
	};
}