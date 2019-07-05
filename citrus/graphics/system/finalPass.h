#pragma once

#include "citrus/graphics/system/instance.h"

namespace citrus::graphics {
	class finalPassShader {
		friend class instance;

		instance& _inst;
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSet _set;

		uint32_t _width, _height;

		bool _good;

		vector<VkFramebuffer> targets;
		vector<VkCommandBuffer> cbufs;
	public:
		void fillCommandBuffer(uint32_t frameIndex, VkDescriptorImageInfo colorInfo, VkDescriptorImageInfo depthInfo);

		//submit to graphics queue
		void submit(uint32_t targetIndex, VkSemaphore wait, VkSemaphore signal);
		void submit(uint32_t targetIndex, vector<VkSemaphore> waits, VkSemaphore signal);

		void initDescriptorLayouts();

		//constructs final pass compositor shader
		//views : swapchain image views to render to, one target created per view
		finalPassShader(instance& inst, vector<VkImageView> views, uint32_t width, uint32_t height, string vertLoc, string fragLoc);
		~finalPassShader();
	};
}