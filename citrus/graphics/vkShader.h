#pragma once

#include "citrus/graphics/instance.h"
#include "citrus/graphics/mesh.h"

namespace citrus::graphics {
	
	class vkShader {
		friend class instance;

		instance& _inst;
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		
		uint32_t _width, _height;
		
	public:
		
		//these are terrible
		void beginBufferAndRenderPass(ctFBO fbo);
		void bindPipelineAndDraw(ctFBO fbo);
		void endRenderPassAndBuffer(ctFBO fbo);
		
		void bindTexture(uint32_t target, ctTexture const& tex);
		
		vector<ctFBO> targets;

		vkShader(instance& inst, meshDescription const& desc, vector<VkImageView> views, uint32_t width, uint32_t height, bool useDepth, string vertLoc, string geomLoc, string fragLoc);
		~vkShader();
	};
}
