#include <citrus/util.h>

#include <citrus/graphics/vkShader.h>

namespace citrus::graphics {
	void vkShader::beginBufferAndRenderPass(ctFBO fbo) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(fbo.cbf, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.framebuffer = fbo.fbo;

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _inst._extent;

		VkClearValue clearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(fbo.cbf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void vkShader::bindPipelineAndDraw(ctFBO fbo) {
		vkCmdBindPipeline(fbo.cbf, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindDescriptorSets(fbo.cbf, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &fbo.set, 0, nullptr);
		vkCmdDraw(fbo.cbf, 3, 1, 0, 0);
	}
	void vkShader::endRenderPassAndBuffer(ctFBO fbo) {
		vkCmdEndRenderPass(fbo.cbf);
		if (vkEndCommandBuffer(fbo.cbf) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	vkShader::vkShader(instance& inst, meshDescription const& desc, vector<VkImageView> views, uint32_t width, uint32_t height, string vertLoc, string geomLoc, string fragLoc) : _inst(inst), _width(width), _height(height) {

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
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
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

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
		
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
		
		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount = 1;
		descriptorLayoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(_inst._device, &descriptorLayoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if(vkCreatePipelineLayout(inst._device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = _inst._surfaceFormat.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

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

		if(vkCreateRenderPass(_inst._device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
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
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if(vkCreateGraphicsPipelines(_inst._device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS) {
			
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		if(vertModule != VK_NULL_HANDLE) vkDestroyShaderModule(inst._device, vertModule, nullptr);
		if(geomModule != VK_NULL_HANDLE) vkDestroyShaderModule(inst._device, geomModule, nullptr);
		if(fragModule != VK_NULL_HANDLE) vkDestroyShaderModule(inst._device, fragModule, nullptr);
		
		VkDescriptorPoolSize poolSize = { };
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(_inst._swapChainImageViews.size());
		
		VkDescriptorPoolCreateInfo poolInfo = { };
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(views.size());
		
		if (vkCreateDescriptorPool(_inst._device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
		
		//allocate 1 descriptor set per fbo
		std::vector<VkDescriptorSetLayout> layouts(views.size(), _descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(views.size());
		allocInfo.pSetLayouts = layouts.data();
		
		vector<VkDescriptorSet> descriptorHandles;
		descriptorHandles.resize(views.size());
		if (vkAllocateDescriptorSets(_inst._device, &allocInfo, descriptorHandles.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for(int i = 0; i < views.size(); i++) {
			VkImageView view = views[i];
			VkImageView attachments[] = {
				view
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = _width;
			framebufferInfo.height = _height;
			framebufferInfo.layers = 1;

			VkFramebuffer fbo;
			if(vkCreateFramebuffer(_inst._device, &framebufferInfo, nullptr, &fbo) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
			
			VkCommandBuffer buf;
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = _inst._commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			if(vkAllocateCommandBuffers(_inst._device, &allocInfo, &buf) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}

			uint64_t off = _inst.uniformMem.alloc(sizeof(mat4));

			VkDescriptorBufferInfo bufferInfo = { };
			bufferInfo.buffer = _inst.uniformMem.buf;
			bufferInfo.offset = off;
			bufferInfo.range = sizeof(mat4);
			
			VkWriteDescriptorSet descriptorWrite = { };
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorHandles[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(_inst._device, 1, &descriptorWrite, 0, nullptr);

			ctFBO res = { };
			res.fbo = fbo;
			res.cbf = buf;
			res.set = descriptorHandles[i];
			res.off = off;

			targets.push_back(res);
		}
	}
	vkShader::~vkShader() {
		if(_descriptorSetLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(_inst._device, _descriptorSetLayout, nullptr);
		if(_descriptorPool != VK_NULL_HANDLE) vkDestroyDescriptorPool(_inst._device, _descriptorPool, nullptr);
		for(int i = 0; i < targets.size(); i++) {			
			vkDestroyFramebuffer(_inst._device, targets[i].fbo, nullptr);
			vkFreeCommandBuffers(_inst._device, _inst._commandPool, 1, &targets[i].cbf);
		}
		if(_pipeline != VK_NULL_HANDLE) vkDestroyPipeline(_inst._device, _pipeline, nullptr);
		if(_renderPass != VK_NULL_HANDLE) vkDestroyRenderPass(_inst._device, _renderPass, nullptr);
		if(_pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(_inst._device, _pipelineLayout, nullptr);
	}
}