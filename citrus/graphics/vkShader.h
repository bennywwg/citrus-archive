#pragma once

#include "citrus/graphics/instance.h"
#include "citrus/graphics/mesh.h"

namespace citrus::graphics {

	struct invocationData {
		VkFramebuffer			fbo;
		VkDescriptorSet			set;
		ctDynamicOffsetBuffer	buf;
	};
	
	class dynamicOffsetMeshShader {
	public:
		friend class instance;

		instance& inst;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		VkPipeline pipeline;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		
		uint32_t width, height;

		uint64_t realItemSize;
		
		void beginShader(uint32_t frameIndex, VkCommandBuffer buf);
		void endShader(uint32_t frameIndex, VkCommandBuffer buf);
		
		void bindTexture(uint32_t target, ctTexture const& tex);
		
		vector<invocationData> targets;

		/*
		 * maxItems = max number of mesh instances that can be drawn with an invocation
		 * itemSize = size in bytes of one item
		 * staticSize = size of data used per invocation (for things like frame index, time, etc. not per mesh)
		*/
		dynamicOffsetMeshShader(instance& inst,
			meshDescription const& desc,
			vector<VkImageView> views,
			uint32_t width, uint32_t height, bool useDepth,
			uint64_t maxItems, uint64_t itemSize,
			string vertLoc, string geomLoc, string fragLoc);
		~dynamicOffsetMeshShader();
	};
}
