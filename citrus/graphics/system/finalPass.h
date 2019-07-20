#pragma once

#include "citrus/graphics/system/renderSystem.h"
#include "citrus/graphics/system/sysNode.h"
#include "citrus/graphics/window.h"
#include "citrus/graphics/system/meshPass.h"

namespace citrus::graphics {
	class finalPass : public sysNode {
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSet _set;

		vector<VkFramebuffer> targets;
		vector<VkCommandBuffer> cbufs;

		meshPass&				prevPass;
		VkSemaphore				frameReadySem;
		window&					win;
		uint32_t				frameIndex;
	public:
		void fillCommandBuffer(uint32_t frameIndex, VkDescriptorImageInfo colorInfo, VkDescriptorImageInfo depthInfo, VkDescriptorImageInfo indexInfo);

		//submit to graphics queue
		void submit(uint32_t targetIndex, vector<VkSemaphore> waits);

		void initDescriptorsAndLayouts();

		void preRender(uint32_t const& threadCount);
		void renderPartial(uint32_t const& threadIndex);
		void postRender(uint32_t const& threadCount);

		//constructs final pass compositor shader
		//views : swapchain image views to render to, one target created per view
		finalPass(system& sys, window& win, meshPass& prev, fpath vertLoc, fpath fragLoc);
		~finalPass();
	};
}