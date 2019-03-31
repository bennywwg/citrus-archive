#include "citrus/util.h"

#include "citrus/graphics/vkShader.h"

#include <iostream>

namespace citrus::graphics {
	void dynamicOffsetMeshShader::beginShader(uint32_t frameIndex, VkCommandBuffer buf) {
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = targets[frameIndex].fbo;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = inst._extent;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(buf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}
	void dynamicOffsetMeshShader::endShader(uint32_t frameIndex, VkCommandBuffer buf) {
		vkCmdEndRenderPass(buf);
	}
	void dynamicOffsetMeshShader::bindTexture(uint32_t target, ctTexture const& tex) {
		VkDescriptorImageInfo imageInfo = { };
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = tex.view;
		imageInfo.sampler = tex.samp;
		
		VkWriteDescriptorSet descriptorWrite = { };
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = targets[target].set;
		descriptorWrite.dstBinding = 1;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(inst._device, 1, &descriptorWrite, 0, nullptr);
	}
	
	dynamicOffsetMeshShader::dynamicOffsetMeshShader(instance& inst, meshDescription const& desc,
        vector<VkImageView> views, uint32_t width, uint32_t height, bool useDepth,
		uint64_t maxItems, uint64_t itemSize,
        string vertLoc, string geomLoc, string fragLoc) : inst(inst), width(width), height(height) {

		VkShaderModule vertModule = VK_NULL_HANDLE;
		VkShaderModule geomModule = VK_NULL_HANDLE;
		VkShaderModule fragModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo vertCreateInfo = { };
		VkPipelineShaderStageCreateInfo geomCreateInfo = { };
		VkPipelineShaderStageCreateInfo fragCreateInfo = { };
		try {
			{
				string src = util::loadEntireFile(vertLoc);

				VkShaderModuleCreateInfo createInfo = { };
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = src.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

				if(vkCreateShaderModule(inst._device, &createInfo, nullptr, &vertModule) != VK_SUCCESS) {
					vertModule = VK_NULL_HANDLE;
					throw std::runtime_error("Failed to create vert shader module");
				}

				vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertCreateInfo.module = vertModule;
				vertCreateInfo.pName = "main";
			}
			if(!geomLoc.empty()) {
				string src = util::loadEntireFile(geomLoc);

				VkShaderModuleCreateInfo createInfo = { };
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = src.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

				if(vkCreateShaderModule(inst._device, &createInfo, nullptr, &geomModule) != VK_SUCCESS) {
					geomModule = VK_NULL_HANDLE;
					throw std::runtime_error("Failed to create geom shader module");
				}

				geomCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				geomCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				geomCreateInfo.module = vertModule;
				geomCreateInfo.pName = "main";
			}
			{
				string src = util::loadEntireFile(fragLoc);

				VkShaderModuleCreateInfo createInfo = { };
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = src.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

				if(vkCreateShaderModule(inst._device, &createInfo, nullptr, &fragModule) != VK_SUCCESS) {
					fragModule = VK_NULL_HANDLE;
					throw std::runtime_error("Failed to create frag shader module");
				}

				fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragCreateInfo.module = fragModule;
				fragCreateInfo.pName = "main";
			}
		} catch(std::runtime_error& rt) {
			std::cout << "yooooo!\n";
		}

		vector<VkPipelineShaderStageCreateInfo> stages = geomLoc.empty() ?
			vector<VkPipelineShaderStageCreateInfo> {vertCreateInfo, fragCreateInfo } :
			vector<VkPipelineShaderStageCreateInfo> {vertCreateInfo, geomCreateInfo, fragCreateInfo};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(desc.bindings.size());
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(desc.attribs.size());
		vertexInputInfo.pVertexBindingDescriptions = desc.bindings.data();
		vertexInputInfo.pVertexAttributeDescriptions = desc.attribs.data();
		
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = {width, height};

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = { };
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		/*colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/

		VkPipelineColorBlendStateCreateInfo colorBlending = { };
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
		
		VkDescriptorSetLayoutBinding uboLayoutBinding = { };
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		VkDescriptorSetLayoutBinding texLayoutBinding = { };
		texLayoutBinding.binding = 1;
		texLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texLayoutBinding.descriptorCount = 1;
		texLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings = { uboLayoutBinding, texLayoutBinding };
		
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		descriptorLayoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(inst._device, &descriptorLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if(vkCreatePipelineLayout(inst._device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkAttachmentDescription colorAttachment = { };
		colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        VkAttachmentDescription depthAttachment = { };
        depthAttachment.format = inst.findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
		VkAttachmentReference colorAttachmentRef = { };
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference depthAttachmentRef = { };
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = useDepth ? &depthAttachmentRef : nullptr;
        
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if(vkCreateRenderPass(inst._device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
		
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
		pipelineInfo.pStages = stages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if(vkCreateGraphicsPipelines(inst._device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
			
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		if(vertModule != VK_NULL_HANDLE) vkDestroyShaderModule(inst._device, vertModule, nullptr);
		if(geomModule != VK_NULL_HANDLE) vkDestroyShaderModule(inst._device, geomModule, nullptr);
		if(fragModule != VK_NULL_HANDLE) vkDestroyShaderModule(inst._device, fragModule, nullptr);
		
		std::array<VkDescriptorPoolSize, 2> poolSizes = { };
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(views.size());
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(views.size());
		
		VkDescriptorPoolCreateInfo poolInfo = { };
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(views.size());
		
		if (vkCreateDescriptorPool(inst._device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
		
		//allocate 1 descriptor set per fbo
		std::vector<VkDescriptorSetLayout> layouts(views.size(), descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(views.size());
		allocInfo.pSetLayouts = layouts.data();
		
		vector<VkDescriptorSet> descriptorHandles;
		descriptorHandles.resize(views.size());
		if (vkAllocateDescriptorSets(inst._device, &allocInfo, descriptorHandles.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		realItemSize = util::roundUpAlign(itemSize, inst.minUniformBufferOffsetAlignment());
		uint64_t totalSize = maxItems * realItemSize;
		for(int i = 0; i < views.size(); i++) {
			VkImageView view = views[i];
			VkImageView attachments[] = {
				view
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = width;
			framebufferInfo.height = height;
			framebufferInfo.layers = 1;

			VkFramebuffer fbo;
			if(vkCreateFramebuffer(inst._device, &framebufferInfo, nullptr, &fbo) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
			
			invocationData res = { };
			res.fbo = fbo;
			res.set = descriptorHandles[i];
			res.buf = inst.createDynamicOffsetBuffer(totalSize);

			VkDescriptorBufferInfo bufferInfo = { };
			bufferInfo.buffer = res.buf.buf;
			bufferInfo.offset = 0;
			bufferInfo.range = realItemSize;
			
			VkWriteDescriptorSet descriptorWrite = { };
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = res.set;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(inst._device, 1, &descriptorWrite, 0, nullptr);

			targets.push_back(res);
		}
	}
	dynamicOffsetMeshShader::~dynamicOffsetMeshShader() {
		if(descriptorSetLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(inst._device, descriptorSetLayout, nullptr);
		if(descriptorPool != VK_NULL_HANDLE) vkDestroyDescriptorPool(inst._device, descriptorPool, nullptr);
		for(int i = 0; i < targets.size(); i++) {
			inst.destroyDynamicOffsetBuffer(targets[i].buf);
			vkDestroyFramebuffer(inst._device, targets[i].fbo, nullptr);
		}
		if(pipeline != VK_NULL_HANDLE) vkDestroyPipeline(inst._device, pipeline, nullptr);
		if(renderPass != VK_NULL_HANDLE) vkDestroyRenderPass(inst._device, renderPass, nullptr);
		if(pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(inst._device, pipelineLayout, nullptr);
	}
}
