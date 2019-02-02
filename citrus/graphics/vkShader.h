#pragma once

#include <citrus/graphics/instance.h>

namespace citrus::graphics {
	class vkShader {
		friend class instance;

		instance& _inst;
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;

		vector<VkFramebuffer> _framebuffers;
		vector<VkCommandBuffer> _buffers;
		
	public:

		void beginAll();
		void drawAll(int verts);
		void endAll();

		vkShader(instance& inst, meshDescription const& desc, vector<VkImageView> fbos, uint32_t width, uint32_t height, string vertLoc, string geomLoc, string fragLoc);
		~vkShader();
	};
}
