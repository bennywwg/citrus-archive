#pragma once

#include <graphics/vulkan/instance.h>

namespace citrus::graphics {
	class vkShader {
		instance& _inst;
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;

		vector<VkFramebuffer> _framebuffers;
		vector<VkCommandBuffer> _buffers;
		
	public:

		vkShader(instance& inst, vector<VkImageView> fbos, uint32_t width, uint32_t height, string vertLoc, string geomLoc, string fragLoc);
		~vkShader();
	};
}