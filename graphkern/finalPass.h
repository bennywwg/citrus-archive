#pragma once

#include "window.h"
#include "renderSystem.h"
#include "sysNode.h"
#include "meshPass.h"
#include "runtimeResource.h"

namespace citrus {
	class finalPass : public sysNode {
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSet _set;

		std::vector<VkFramebuffer> targets;
		std::vector<VkCommandBuffer> cbufs;

		struct uniformBlock {
			mat4 vp;
			vec4 cameraPos;
			float ncp;
			float fcp;
			uint32_t widthPX;
			uint32_t heightPX;
		};
		buffer					uniformData;
		
		frameStore&				fStore;
		VkSemaphore				frameReadySem, frameDoneSem;
		window&					win;
		uint32_t				frameIndex;
	public:
		void fillCommandBuffer(uint32_t frameIndex, VkDescriptorImageInfo colorInfo, VkDescriptorImageInfo indexInfo, VkDescriptorImageInfo depthInfo);

		//submit to graphics queue
		void submit(uint32_t targetIndex, std::vector<VkSemaphore> waits);

		void initDescriptorsAndLayouts();

		void preRender(uint32_t const& threadCount);
		void renderPartial(uint32_t const& threadIndex);
		void postRender(uint32_t const& threadCount);

		//constructs final pass compositor shader
		//views : swapchain image views to render to, one target created per view
		finalPass(renderSystem& sys, window& win, frameStore& prev, fpath vertLoc, fpath fragLoc);
		~finalPass();
	};
}