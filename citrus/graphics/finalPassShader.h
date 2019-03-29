#pragma once

#include "citrus/graphics/instance.h"

namespace citrus::graphics {
    class finalPassShader {
		friend class instance;

		instance& _inst;
		VkPipelineLayout _pipelineLayout;
		VkRenderPass _renderPass;
		VkPipeline _pipeline;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkDescriptorPool _descriptorPool;
		
		uint32_t _width, _height;
        
        bool _good;
        
		vector<ctFBO> targets; 
		vector<VkCommandBuffer> cbufs;
	public:
        void fillCommandBuffer(uint32_t frameIndex, ctTexture rtex);
        
        //submit to graphics queue
		void submit(uint32_t targetIndex, VkSemaphore wait, VkSemaphore signal);
		void submit(uint32_t targetIndex, vector<VkSemaphore> waits, VkSemaphore signal);
		
        //constructs final pass compositor shader
        //views : swapchain image views to render to, one target created per view
		finalPassShader(instance& inst, vector<VkImageView> views, uint32_t width, uint32_t height, string vertLoc, string fragLoc);
		~finalPassShader();
	};
}
