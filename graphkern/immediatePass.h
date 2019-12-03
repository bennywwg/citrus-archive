#pragma once

#include "renderSystem.h"
#include "runtimeResource.h"
#include "sysNode.h"
#include "util.h"

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

		struct grouping {
			mat4			tr;
			vec3			color;
			std::vector<vec3>	data;
			std::vector<vec2>	uvdata;
			bool			pixelspace;
			/// add a mesh presenting a string to this grouping
			/// fills in any missing uv data if needed
			void addText(string text, int px, ivec2 pos = ivec2(0, 0));
			void addCube(vec3 halfDims);
			void addTorus(float radMajor, float radMinor, uint32_t majSegs, uint32_t minSegs);
			void addArrow(float rad, float len, uint32_t majorCount);
		};

		struct uniformBlock {
			mat4 mvp;
			vec4 color;
			uint32_t index;
		};

		std::vector<grouping>	groupings;

		bool				active = true;

		immediatePass(renderSystem& sys, frameStore* fstore, fpath const& vert, fpath const& frag, bool transitionToRead);
		~immediatePass();
	};
}