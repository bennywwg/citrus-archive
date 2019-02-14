#pragma once

#include <citrus/graphics/instance.h>
#include <citrus/graphics/mesh.h>

namespace citrus::graphics {
	
	struct vkFBO {
		VkFramebuffer fbo;
		VkCommandBuffer cbf;
	};
	
	class vkShader {
		friend class instance;

		instance& _inst;
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;
		
		uint32_t _width, _height;

		vector<VkFramebuffer> _framebuffers;
		vector<VkCommandBuffer> _buffers;
		
	public:

		void beginAll();
		void drawAll(int verts);
		void endAll();
		
		vkFBO createFBO(VkImageView view);
		void freeFBO(vkFBO fbo);

		vkShader(instance& inst, meshDescription const& desc, uint32_t width, uint32_t height, string vertLoc, string geomLoc, string fragLoc);
		~vkShader();
	};
}
