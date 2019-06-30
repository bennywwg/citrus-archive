#include "citrus/graphics/renderSystem.h"
#include <fstream>

namespace citrus::graphics {
	passMaterial::passMaterial(system & sys, string const& vertLoc, string const& fragLoc) : sys(sys) {
		VkShaderModule vertModule, fragModule;
		VkPipelineShaderStageCreateInfo vertInfo, fragInfo;
		{
			string src = util::loadEntireFile(vertLoc);

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = src.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

			if (vkCreateShaderModule(sys.inst._device, &createInfo, nullptr, &vertModule) != VK_SUCCESS) {
				vertModule = VK_NULL_HANDLE;
				throw std::runtime_error("Failed to create vert shader module");
			}

			vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertInfo.module = vertModule;
			vertInfo.pName = "main";
		}
		{
			string src = util::loadEntireFile(fragLoc);

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = src.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

			if (vkCreateShaderModule(sys.inst._device, &createInfo, nullptr, &fragModule) != VK_SUCCESS) {
				fragModule = VK_NULL_HANDLE;
				throw std::runtime_error("Failed to create frag shader module");
			}

			fragInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragInfo.module = fragModule;
			fragInfo.pName = "main";
		}
	}

	void passMateriall::initializePipeline() {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo uboLayoutInfo = {};
		uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		uboLayoutInfo.bindingCount = 1;
		uboLayoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(sys.inst._device, &uboLayoutInfo, nullptr, &uboLayout) != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout");

		VkDescriptorSetLayoutBinding texLayoutBinding = {};
		texLayoutBinding.binding = 0;
		texLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texLayoutBinding.descriptorCount = (uint32_t)texCount;
		texLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo texLayoutInfo = {};
		texLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		texLayoutInfo.bindingCount = 1;
		texLayoutInfo.pBindings = &texLayoutBinding;

		if (vkCreateDescriptorSetLayout(inst._device, &texLayoutInfo, nullptr, &texLayout) != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout");
	}

	void system::initializeAttribsBindings() {
		VkFormat formats[] = {
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R32_SFLOAT
		};
		uint32_t strides[] = {
			sizeof(vec3),
			sizeof(vec3),
			sizeof(vec3),
			sizeof(vec2),
			sizeof(int32_t),
			sizeof(int32_t),
			sizeof(float),
			sizeof(float)
		};
		for (int i = 0; i < animated_vertex_inputs; i++) {
			attribs[i].format = formats[i];
			attribs[i].binding = i;
			attribs[i].location = i;
			attribs[i].offset = 0;

			bindings[i].binding = i;
			bindings[i].stride = strides[i];
			bindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		}
	}
	void system::initializeDescriptorLayouts(int texCount) {
		
	}
	void system::freeDescriptorLayouts() {
		vkDestroyDescriptorSetLayout(inst._device, uboLayout, nullptr);
		vkDestroyDescriptorSetLayout(inst._device, texLayout, nullptr);
	}
	void system::initializePipelineLayout() {
		VkPushConstantRange pcRange = {};
		pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pcRange.size = sizeof(vert_pcData);
		pcRange.offset = 0;

		VkPushConstantRange frag_pcRange = {};
		frag_pcRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_pcRange.size = sizeof(frag_pcData);
		frag_pcRange.offset = sizeof(vert_pcData);

		VkPushConstantRange ranges[2] = { pcRange, frag_pcRange };

		VkDescriptorSetLayout setLayouts[2] = { uboLayout, texLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 2;
		pipelineLayoutInfo.pPushConstantRanges = ranges;
		pipelineLayoutInfo.setLayoutCount = 2;
		pipelineLayoutInfo.pSetLayouts = setLayouts;

		if (vkCreatePipelineLayout(inst._device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout");
	}
	void system::freePipelineLayout() {
		vkDestroyPipelineLayout(inst._device, pipelineLayout, nullptr);
	}
	void system::initializeDescriptorSets(int texCount) {
		VkDescriptorPoolSize uboSize = {};
		uboSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		uboSize.descriptorCount = SWAP_FRAMES;

		VkDescriptorPoolCreateInfo uboPoolInfo = {};
		uboPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		uboPoolInfo.poolSizeCount = 1;
		uboPoolInfo.pPoolSizes = &uboSize;
		uboPoolInfo.maxSets = SWAP_FRAMES;

		if (vkCreateDescriptorPool(inst._device, &uboPoolInfo, nullptr, &uboPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create ubo descriptor pool!");
		}

		VkDescriptorSetLayout uboLayouts[4] = { uboLayout, uboLayout, uboLayout, uboLayout };
		VkDescriptorSetAllocateInfo uboInfo = {};
		uboInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		uboInfo.descriptorPool = uboPool;
		uboInfo.descriptorSetCount = SWAP_FRAMES;
		uboInfo.pSetLayouts = uboLayouts;

		if (vkAllocateDescriptorSets(inst._device, &uboInfo, uboSets) != VK_SUCCESS) throw std::runtime_error("failed to allocated ubo sets");

		VkDescriptorPoolSize texSize = {};
		texSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texSize.descriptorCount = texCount;

		VkDescriptorPoolCreateInfo texPoolInfo = {};
		texPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		texPoolInfo.poolSizeCount = 1;
		texPoolInfo.pPoolSizes = &texSize;
		texPoolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(inst._device, &texPoolInfo, nullptr, &texPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture descriptor pool!");
		}

		VkDescriptorSetAllocateInfo texInfo = {};
		texInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		texInfo.descriptorPool = texPool;
		texInfo.descriptorSetCount = 1;
		texInfo.pSetLayouts = &texLayout;

		if (vkAllocateDescriptorSets(inst._device, &texInfo, &texSet) != VK_SUCCESS) throw std::runtime_error("failed to allocated texture sets");
	}
	void system::freeDescriptorSets() {
		//if (vkFreeDescriptorSets(inst._device, uboPool, 2, uboSets) != VK_SUCCESS) throw std::runtime_error("failed to free ubo descriptor sets");
		vkDestroyDescriptorPool(inst._device, uboPool, nullptr);
		//if (vkFreeDescriptorSets(inst._device, texPool, 1, &texSet) != VK_SUCCESS) throw std::runtime_error("failed to free tex descriptor set");
		vkDestroyDescriptorPool(inst._device, texPool, nullptr);
	}
	void system::initializeRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = inst.findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(inst._device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");
	}
	void system::freeRenderPass() {
		vkDestroyRenderPass(inst._device, renderPass, nullptr);
	}
	void system::initializePipeline(string vs, string fs) {
		VkShaderModule vmod = {}, fmod = {};
		VkPipelineShaderStageCreateInfo vertInfo = {}, fragInfo = {};
		createModules(vs, fs, vmod, fmod, vertInfo, fragInfo);

		VkPipelineShaderStageCreateInfo stages[] = { vertInfo, fragInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = animated_vertex_inputs;
		vertexInputInfo.vertexAttributeDescriptionCount = animated_vertex_inputs;
		vertexInputInfo.pVertexBindingDescriptions = bindings;
		vertexInputInfo.pVertexAttributeDescriptions = attribs;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)inst.width;
		viewport.height = (float)inst.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { inst.width, inst.height };

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

		VkPipelineDepthStencilStateCreateInfo depthStateInfo = {};
		depthStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStateInfo.stencilTestEnable = VK_FALSE;
		depthStateInfo.minDepthBounds = 0.0f;
		depthStateInfo.maxDepthBounds = 1.0f;
		depthStateInfo.depthWriteEnable = VK_TRUE;
		depthStateInfo.depthTestEnable = VK_TRUE;
		depthStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStateInfo.back = {};
		depthStateInfo.front = {};
		depthStateInfo.depthBoundsTestEnable = VK_FALSE;

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = stages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStateInfo; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(inst._device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");

		vkDestroyShaderModule(inst._device, vertInfo.module, nullptr);
		vkDestroyShaderModule(inst._device, fragInfo.module, nullptr);
	}
	void system::freePipeline() {
		vkDestroyPipeline(inst._device, pipeline, nullptr);
	}

	void system::createFramebufferData() {
		VkImageCreateInfo colorInfo = {};
		colorInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colorInfo.imageType = VK_IMAGE_TYPE_2D;
		colorInfo.extent.width = inst.width;
		colorInfo.extent.height = inst.height;
		colorInfo.extent.depth = 1;
		colorInfo.mipLevels = 1;
		colorInfo.arrayLayers = 1;
		colorInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		colorInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		colorInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		colorInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkImageCreateInfo depthInfo = {};
		depthInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthInfo.imageType = VK_IMAGE_TYPE_2D;
		depthInfo.extent.width = inst.width;
		depthInfo.extent.height = inst.height;
		depthInfo.extent.depth = 1;
		depthInfo.mipLevels = 1;
		depthInfo.arrayLayers = 1;
		depthInfo.format = inst.findDepthFormat();
		depthInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		depthInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		depthInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateImage(inst._device, &colorInfo, nullptr, &frames[i].color) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			if (vkCreateImage(inst._device, &depthInfo, nullptr, &frames[i].depth) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements colorRequirements, depthRequirements;
			vkGetImageMemoryRequirements(inst._device, frames[i].color, &colorRequirements);
			vkGetImageMemoryRequirements(inst._device, frames[i].depth, &depthRequirements);

			VkMemoryAllocateInfo colorAllocInfo = {}, depthAllocInfo = {};
			colorAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			colorAllocInfo.memoryTypeIndex = inst.findMemoryType(colorRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			colorAllocInfo.allocationSize = colorRequirements.size;
			depthAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			depthAllocInfo.memoryTypeIndex = inst.findMemoryType(depthRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			depthAllocInfo.allocationSize = depthRequirements.size;
			vkAllocateMemory(inst._device, &colorAllocInfo, nullptr, &frames[i].colorMem);
			vkAllocateMemory(inst._device, &depthAllocInfo, nullptr, &frames[i].depthMem);

			vkBindImageMemory(inst._device, frames[i].color, frames[i].colorMem, 0);
			vkBindImageMemory(inst._device, frames[i].depth, frames[i].depthMem, 0);

			VkImageViewCreateInfo colorViewInfo = {}, depthViewInfo = {};
			colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			colorViewInfo.image = frames[i].color;
			colorViewInfo.subresourceRange.baseMipLevel = 0;
			colorViewInfo.subresourceRange.baseArrayLayer = 0;
			colorViewInfo.subresourceRange.levelCount = 1;
			colorViewInfo.subresourceRange.layerCount = 1;
			colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthViewInfo.format = inst.findDepthFormat();
			depthViewInfo.image = frames[i].depth;
			depthViewInfo.subresourceRange.baseMipLevel = 0;
			depthViewInfo.subresourceRange.baseArrayLayer = 0;
			depthViewInfo.subresourceRange.levelCount = 1;
			depthViewInfo.subresourceRange.layerCount = 1;
			depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			vkCreateImageView(inst._device, &colorViewInfo, nullptr, &frames[i].colorView);
			vkCreateImageView(inst._device, &depthViewInfo, nullptr, &frames[i].depthView);

			VkImageView views[2] = { frames[i].colorView, frames[i].depthView };

			VkSamplerCreateInfo sampInfo = {};
			sampInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampInfo.magFilter = VK_FILTER_NEAREST;
			sampInfo.minFilter = VK_FILTER_NEAREST;
			sampInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampInfo.anisotropyEnable = VK_FALSE;
			sampInfo.maxAnisotropy = 1;
			sampInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampInfo.unnormalizedCoordinates = VK_FALSE;
			sampInfo.compareEnable = VK_FALSE;
			sampInfo.compareOp = VK_COMPARE_OP_NEVER;
			sampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			sampInfo.mipLodBias = 0.0f;
			sampInfo.minLod = 0.0f;
			sampInfo.maxLod = 0.0f;

			vkCreateSampler(inst._device, &sampInfo, nullptr, &frames[i].colorSamp);
			frames[i].depthSamp = VK_NULL_HANDLE;

			/*VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.width = inst.width;
			fbInfo.height = inst.height;
			fbInfo.layers = 1;
			fbInfo.renderPass = renderPass;
			fbInfo.attachmentCount = 2;
			fbInfo.pAttachments = views;

			vkCreateFramebuffer(inst._device, &fbInfo, nullptr, &frames[i].frameBuffer);*/
		}
	}
	void system::freeFramebufferData() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			//vkDestroyFramebuffer(inst._device, frames[i].frameBuffer, nullptr);
			//vkDestroySampler(inst._device, frames[i].depthSamp, nullptr);
			vkDestroySampler(inst._device, frames[i].colorSamp, nullptr);
			vkDestroyImageView(inst._device, frames[i].colorView, nullptr);
			vkDestroyImageView(inst._device, frames[i].depthView, nullptr);
			vkDestroyImage(inst._device, frames[i].color, nullptr);
			vkDestroyImage(inst._device, frames[i].depth, nullptr);
			vkFreeMemory(inst._device, frames[i].colorMem, nullptr);
			vkFreeMemory(inst._device, frames[i].depthMem, nullptr);
		}
	}
	void system::loadTextures() {
		uint64_t addr = 0;
		uint32_t typeBits = 0;
		for (int i = 0; i < texturePaths.size(); i++) {
			textures.push_back({});

			textures[i].data = std::move(image4b(texturePaths[i]));

			textures[i].format = VK_FORMAT_R8G8B8A8_UNORM;

			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			info.extent.width = textures[i].data.width();
			info.extent.height = textures[i].data.height();
			info.extent.depth = 1;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.format = VK_FORMAT_R8G8B8A8_UNORM;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.samples = VK_SAMPLE_COUNT_1_BIT;

			vkCreateImage(inst._device, &info, nullptr, &textures[i].img);

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(inst._device, textures[i].img, &memRequirements);
			if (typeBits == 0) typeBits = memRequirements.memoryTypeBits;
			else if (typeBits != memRequirements.memoryTypeBits) throw std::runtime_error("different image memory requirements :(");

			addr = util::roundUpAlign(addr, memRequirements.alignment);
			textures[i].off = addr;
			addr += memRequirements.size;
		}

		VkMemoryAllocateInfo memInfo = {};
		memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memInfo.allocationSize = addr;
		memInfo.memoryTypeIndex = inst.findMemoryType(typeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkAllocateMemory(inst._device, &memInfo, nullptr, &textureMem);

		for (int i = 0; i < texturePaths.size(); i++) {
			vkBindImageMemory(inst._device, textures[i].img, textureMem, textures[i].off);

			inst.pipelineBarrierLayoutChange(textures[i].img,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0, VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
			uint64_t stagingAddr = inst.stagingMem.alloc(textures[i].data.size());
			inst.mapUnmapMemory(inst.stagingMem.mem, textures[i].data.size(), stagingAddr, textures[i].data.data());
			inst.copyBufferToImage(inst.stagingMem.buf, stagingAddr, textures[i].img, textures[i].data.width(), textures[i].data.height());
			inst.stagingMem.free(stagingAddr);
			inst.pipelineBarrierLayoutChange(textures[i].img, VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			viewInfo.image = textures[i].img;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			vkCreateImageView(inst._device, &viewInfo, nullptr, &textures[i].view);

			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_NEAREST;
			samplerInfo.minFilter = VK_FILTER_NEAREST;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.maxAnisotropy = 1;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;

			vkCreateSampler(inst._device, &samplerInfo, nullptr, &textures[i].samp);
		}

		vector<VkDescriptorImageInfo> imageInfos;
		for (int i = 0; i < texturePaths.size(); i++) {
			imageInfos.push_back({});
			imageInfos[i].sampler = textures[i].samp;
			imageInfos[i].imageView = textures[i].view;
			imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = (uint32_t)texturePaths.size();
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.dstBinding = 0;
		write.dstSet = texSet;
		write.pImageInfo = imageInfos.data();

		vkUpdateDescriptorSets(inst._device, 1, &write, 0, nullptr);
	}
	void system::freeTextures() {
		vkFreeMemory(inst._device, textureMem, nullptr);
		for (int t = 0; t < textures.size(); t++) {
			vkDestroySampler(inst._device, textures[t].samp, nullptr);
			vkDestroyImageView(inst._device, textures[t].view, nullptr);
			vkDestroyImage(inst._device, textures[t].img, nullptr);
		}
	}

	void system::collectModelInfo() {
		VkBufferCreateInfo vertexInfo = {};
		vertexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vertexInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertexInfo.size = 1024 * 1024; // buffer never actually uses memory

		VkBufferCreateInfo indexInfo = {};
		indexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		indexInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexInfo.size = 1024 * 1024; // buffer never actually uses memory

		VkBuffer tmpVertexBuf, tmpIndexBuf;
		vkCreateBuffer(inst._device, &vertexInfo, nullptr, &tmpVertexBuf);
		vkCreateBuffer(inst._device, &indexInfo, nullptr, &tmpIndexBuf);

		vkGetBufferMemoryRequirements(inst._device, tmpVertexBuf, &vertexRequirements);
		vkGetBufferMemoryRequirements(inst._device, tmpIndexBuf, &indexRequirements);

		vkDestroyBuffer(inst._device, tmpVertexBuf, nullptr);
		vkDestroyBuffer(inst._device, tmpIndexBuf, nullptr);
	}
	void system::loadModels() {
		uint64_t vertexAddr = 0, indexAddr = 0;

		models.resize(modelPaths.size());

		for (int i = 0; i < modelPaths.size(); i++) {
			models[i].m = mesh(modelPaths[i]);
			models[i].desc = models[i].m.getDescription(vertexAddr, vertexRequirements.alignment, indexAddr, indexRequirements.alignment);
			models[i].radius = models[i].data[0].m.getMaxRadius();
		}
	}
	void system::initializeModelData() {
		VkDeviceAddress totalVertexSize = models.back().data.back().desc.nextFree;
		VkDeviceAddress totalIndexSize = models.back().data.back().desc.nextFreeIndex;

		VkBufferCreateInfo vertexInfo = { };
		vertexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vertexInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		vertexInfo.size = totalVertexSize;

		vkCreateBuffer(inst._device, &vertexInfo, nullptr, &vertexBuffer);
		for (int i = 0; i < (sizeof(vertexBuffers) / sizeof(vertexBuffers[0])); i++) vertexBuffers[i] = vertexBuffer;

		VkMemoryRequirements vertexRequirements;
		vkGetBufferMemoryRequirements(inst._device, vertexBuffer, &vertexRequirements);

		VkMemoryAllocateInfo vertexMemInfo = { };
		vertexMemInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vertexMemInfo.allocationSize = vertexRequirements.size;
		vertexMemInfo.memoryTypeIndex = inst.findMemoryType(vertexRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(inst._device, &vertexMemInfo, nullptr, &vertexMemory);

		VkBufferCreateInfo indexInfo = { };
		indexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		indexInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		indexInfo.size = totalIndexSize;

		vkCreateBuffer(inst._device, &indexInfo, nullptr, &indexBuffer);

		VkMemoryRequirements indexRequirements;
		vkGetBufferMemoryRequirements(inst._device, indexBuffer, &indexRequirements);

		VkMemoryAllocateInfo indexMemInfo = { };
		indexMemInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		indexMemInfo.allocationSize = indexRequirements.size;
		indexMemInfo.memoryTypeIndex = inst.findMemoryType(indexRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(inst._device, &indexMemInfo, nullptr, &indexMemory);

		vkBindBufferMemory(inst._device, vertexBuffer, vertexMemory, 0);
		vkBindBufferMemory(inst._device, indexBuffer, indexMemory, 0);

		uint64_t vertexStaging = inst.stagingMem.alloc(totalVertexSize);
		void* vertexScratch = malloc(totalVertexSize);
		for (int i = 0; i < models.size(); i++) {
			models[i].data[0].m.fillVertexData(vertexScratch, models[i].data[0].desc);
		}
		inst.mapUnmapMemory(inst.stagingMem.mem, totalVertexSize, vertexStaging, vertexScratch);
		inst.copyBuffer(inst.stagingMem.buf, vertexBuffer, totalVertexSize, vertexStaging, 0);
		inst.stagingMem.free(vertexStaging);
		free(vertexScratch);

		uint64_t indexStaging = inst.stagingMem.alloc(totalIndexSize);
		void* indexScratch = malloc(totalIndexSize);
		for (int i = 0; i < models.size(); i++) {
			models[i].data[0].m.fillIndexData(indexScratch, models[i].data[0].desc);
		}
		inst.mapUnmapMemory(inst.stagingMem.mem, totalIndexSize, indexStaging, indexScratch);
		inst.copyBuffer(inst.stagingMem.buf, indexBuffer, totalIndexSize, indexStaging, 0);
		inst.stagingMem.free(indexStaging);
		free(indexScratch);
	}
	void system::freeModels() {
		vkFreeMemory(inst._device, vertexMemory, nullptr);
		vkFreeMemory(inst._device, indexMemory, nullptr);
		vkDestroyBuffer(inst._device, vertexBuffer, nullptr);
		vkDestroyBuffer(inst._device, indexBuffer, nullptr);
	}
	void system::loadAnimations() {
		for (int i = 0; i < animationPaths.size(); i++) {
			std::ifstream f(animationPaths[i]);
			animation ani;
			ani.read(f);
			animations.push_back(ani);
		}
		for (int i = 0; i < animations.size(); i++) {
			for (int j = 0; j < models.size(); j++) {
				bool success = models[j].data[0].m.bindAnimation(animations[i]);
				if (success) util::sout("animation bound: model " + std::to_string(j) + " lod 0, ani " + std::to_string(i) + "\n");
			}
		}
	}

	void system::initializeThreads(int threadCount) {
		if (threadCount < 1) threadCount = 1;
		else if (threadCount > 32) threadCount = 32;

		renderThreads.resize(threadCount);
		for (size_t i = 0; i < SWAP_FRAMES; i++) {
			primaryBuffers[i] = VK_NULL_HANDLE;
			waitFences[i] = inst.createFence(true);
			secondaryBuffers[i].resize(renderThreads.size());
		}
		commandPools.resize(renderThreads.size());
		for (size_t t = 0; t < renderThreads.size(); t++) {
			renderGo[t] = false;
			commandPools[t] = inst.createCommandPool();
			for (int i = 0; i < SWAP_FRAMES; i++) {
				secondaryBuffers[i][t] = VK_NULL_HANDLE;
			}
		}

		for (size_t t = 1; t < renderThreads.size(); t++) {
			renderThreads[t] = std::thread(&system::renderFunc, this, t);
		}

		staticRanges.resize(renderThreads.size());
		aniRanges.resize(renderThreads.size());
		for (int i = 0; i < renderThreads.size(); i++) {
			staticRanges[i].resize(staticModels.size());
			aniRanges[i].resize(aniModels.size());
		}
	}
	void system::freeThreads() {
		for (int t = 1; t < renderThreads.size(); t++) {
			renderThreads[t].join();
		}
		stdioDebug("all threads joined\n");

		inst.waitForFence(waitFences[frameIndex_]);
		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (primaryBuffers[i] != VK_NULL_HANDLE) vkFreeCommandBuffers(inst._device, inst._commandPool, 1, &primaryBuffers[i]);
			vkDestroyFence(inst._device, waitFences[i], nullptr);
		}
		for (int t = 0; t < renderThreads.size(); t++) {
			for (int i = 0; i < SWAP_FRAMES; i++) {
				if (secondaryBuffers[i][t] != VK_NULL_HANDLE) vkFreeCommandBuffers(inst._device, commandPools[t], 1, &secondaryBuffers[i][t]);
			}
			vkDestroyCommandPool(inst._device, commandPools[t], nullptr);
		}
	}

	void system::initializeUniformData() {
		uniformAlignment = inst.minUniformBufferOffsetAlignment();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = uniformSize;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateBuffer(inst._device, &bufferInfo, nullptr, &uniformBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create memory buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(inst._device, uniformBuffers[i], &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = inst.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			if (vkAllocateMemory(inst._device, &allocInfo, nullptr, &uniformMemories[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate buffer memory!");
			}

			vkBindBufferMemory(inst._device, uniformBuffers[i], uniformMemories[i], 0);

			vkMapMemory(inst._device, uniformMemories[i], 0, uniformSize, 0, (void**)&uniformMapped[i]);
		}
	}
	void system::freeUniformData() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			vkUnmapMemory(inst._device, uniformMemories[i]);
			vkDestroyBuffer(inst._device, uniformBuffers[i], nullptr);
			vkFreeMemory(inst._device, uniformMemories[i], nullptr);
		}
	}

	void system::sequence(vector<vector<itemInfo>> const& items, vector<vector<itemDrawRange>> & ranges) {
		int totalItems = 0;
		for (int m = 0; m < items.size(); m++)
			totalItems += items[m].size();

		hptime seqStart = hpclock::now();
		for (int t = 0; t < renderThreads.size(); t++)
			for (int m = 0; m < items.size(); m++)
				ranges[t][m].modelIndex = -1;

		int target = totalItems / renderThreads.size() + 1;
		int modelIndex = 0, itemBegin = 0, threadIndex = 0, threadRangeIndex = 0, itemsInThread = 0;
		while (true) {
			if (threadIndex != (renderThreads.size() - 1) && itemsInThread == target) {
				itemsInThread = 0;
				threadIndex++;
				threadRangeIndex = 0;
			}

			if (itemBegin == items[modelIndex].size()) {
				modelIndex++;
				itemBegin = 0;
				if (modelIndex == items.size()) break;
			}

			int toAdd = target;
			if (itemsInThread + toAdd > target) {
				toAdd = target - itemsInThread;
			}
			if (itemBegin + toAdd > items[modelIndex].size()) {
				toAdd = items[modelIndex].size() - itemBegin;
			}

			ranges[threadIndex][threadRangeIndex] = { modelIndex, itemBegin, itemBegin + toAdd };
			threadRangeIndex++;

			itemBegin += toAdd;
			itemsInThread += toAdd;
		}
		stdioDebug("sequencing took " + std::to_string((hpclock::now() - seqStart).count() * 0.000001) + " ms\n");
	}
	void system::renderFunc(int threadIndex) {
		if (threadIndex == 0) throw std::runtime_error("started thread 0 (should happen on main thread)");
		while (!stopped) {
			if (renderGo[threadIndex]) {
				renderPartial(threadIndex);
				renderGo[threadIndex] = false;
			} else {
				//std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
		}
		renderGo[threadIndex] = false;
		stdioDebug("completed thread " + std::to_string(threadIndex) + "\n");
	}
	void system::renderPartial(
		int threadIndex,
		vector<model> const& models,
		vector<vector<itemInfo>> const& items,
		vector<itemDrawRange> const& ranges,
		VkCommandBuffer & buf) {
		stdioDebug("\tstarting partial " + std::to_string(threadIndex) + "\n");
		int vbinds = 0, draws = 0, culls = 0;
		int frameIndex;
		hptime partialStart = hpclock::now();

		{	std::shared_lock<std::shared_mutex> lock(startMut);
			frameIndex = frameIndex_;
		}

		{	std::lock_guard<std::mutex> lock(instMut);
			buf = inst.createCommandBuffer(commandPools[threadIndex], true);
		}

		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		cmdInfo.pInheritanceInfo = &inheritanceInfos[frameIndex];

		vkBeginCommandBuffer(buf, &cmdInfo);

		vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &texSet, 0, nullptr);
		for (int s = 0; s < ranges.size(); s++) {
			itemDrawRange const& dr = ranges[s];
			if (dr.modelIndex == -1) break;

			vkCmdBindVertexBuffers(buf, 0, animated_vertex_inputs, vertexBuffers, aniModels[dr.modelIndex].vertexOffsets);
			vkCmdBindIndexBuffer(buf, indexBuffer, aniModels[dr.modelIndex].indexOffset, VK_INDEX_TYPE_UINT16);
			vbinds++;

			for (int i = dr.itemBegin; i < dr.itemEnd; i++) {
				if (!cullEnabled || frameCull.testSphere(items[dr.modelIndex][i].pos, aniModels[dr.modelIndex].radius)) {
					pcData pushData;
					mat4 modTmp = glm::translate(aniItems[dr.modelIndex][i].pos);
					pushData.vertData.model = modTmp;
					pushData.vertData.mvp = frameVP * modTmp;
					pushData.fragData.texIndex = aniItems[dr.modelIndex][i].texIndex;
					vkCmdPushConstants(buf, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(vert_pcData), &pushData.vertData);
					vkCmdPushConstants(buf, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(vert_pcData), sizeof(frag_pcData), &pushData.fragData);
					uint32_t zero = 0;
					vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &uboSets[frameIndex], 1, &zero);

					vkCmdDrawIndexed(buf, aniModels[dr.modelIndex].indexCount, 1, 0, 0, 0);
					draws++;
				} else {
					culls++;
				}
			}
		}

		vkEndCommandBuffer(buf);

		auto time = (hpclock::now() - partialStart).count();

		stdioDebug("\tpartial " + std::to_string(threadIndex) + " culled " + std::to_string(culls) + "\n");

		stdioDebug("\tpartial " + std::to_string(threadIndex) + " took " + std::to_string(time * 0.000001) + " ms [" + std::to_string(vbinds) + " " + std::to_string(draws) + "]\n");
	}
	void system::render(int frameIndex, VkSemaphore signal, camera const& cam) {
		stdioDebug("starting main\n");
		hptime mainStart = hpclock::now();

		inst.waitForFence(waitFences[frameIndex]);
		inst.resetFence(waitFences[frameIndex]);

		stdioDebug("fence wait took " + std::to_string((hpclock::now() - mainStart).count() * 0.000001) + " ms\n");

		sequence(staticItems, staticRanges);
		sequence(aniItems, aniRanges);

		{ //  prepare frame data here
			std::unique_lock<std::shared_mutex> lock(startMut);

			memset(&inheritanceInfos[frameIndex], 0, sizeof(VkCommandBufferInheritanceInfo));
			inheritanceInfos[frameIndex].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfos[frameIndex].renderPass = renderPass;
			inheritanceInfos[frameIndex].framebuffer = frames[frameIndex].frameBuffer;

			frameIndex_ = frameIndex;

			frameCam = cam;
			frameCull = cam.genFrustrumInfo();
			frameVP = cam.getViewProjectionMatrix();

			cullEnabled = false;
		}

		hptime mainRenderStart = hpclock::now();

		for (int i = 0; i < renderThreads.size(); i++) {
			renderGo[i] = true;
		}

		renderPartial(0);

		renderGo[0] = false;

		while (!renderDone()) {
			//std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		hptime mainRenderDone = hpclock::now();
		stdioDebug("main render all threads took " + std::to_string((mainRenderDone - mainRenderStart).count() * 0.000001) + " ms\n");

		if (primaryBuffers[frameIndex] != VK_NULL_HANDLE) inst.destroyCommandBuffer(primaryBuffers[frameIndex], inst._commandPool);
		primaryBuffers[frameIndex] = inst.createCommandBuffer(inst._commandPool, false);

		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(primaryBuffers[frameIndex], &beginInfo);

		VkClearValue clearValues[2] = { };
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassInfo = { };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = frames[frameIndex].frameBuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = inst._extent;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(primaryBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		vkCmdExecuteCommands(primaryBuffers[frameIndex], renderThreads.size(), secondaryBuffers[frameIndex].data());

		vkCmdEndRenderPass(primaryBuffers[frameIndex]);

		vkEndCommandBuffer(primaryBuffers[frameIndex]);

		VkSubmitInfo subInfo = { };
		subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInfo.commandBufferCount = 1;
		subInfo.pCommandBuffers = &primaryBuffers[frameIndex];
		subInfo.signalSemaphoreCount = 1;
		subInfo.pSignalSemaphores = &signal;

		vkQueueSubmit(inst._graphicsQueue, 1, &subInfo, waitFences[frameIndex]);

		auto time = hpclock::now();
		stdioDebug("submitting all buffers took " + std::to_string((time - mainRenderDone).count() * 0.000001) + " ms\n");
		stdioDebug("main render took " + std::to_string((time - mainStart).count() * 0.000001) + " ms (avg " + std::to_string(RENDER_ITEMS * RENDER_MODELS / ((time - mainStart).count() * 0.000001)) + " items / ms)\n");
	}
	bool system::renderDone() {
		for (int i = 0; i < renderThreads.size(); i++) {
			if (renderGo[i] == true) return false;
		}
		return true;
	}
	void system::postProcess(int frameIndex, int windowSwapIndex, vector<VkSemaphore> waits, VkSemaphore signal) {
		VkDescriptorImageInfo colorInfo = {};
		colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorInfo.imageView = frames[frameIndex].colorView;
		colorInfo.sampler = frames[frameIndex].samp;

		VkDescriptorImageInfo depthInfo = {};
		depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthInfo.imageView = frames[frameIndex].depthView;
		depthInfo.sampler = frames[frameIndex].samp;

		inst._finalPass->fillCommandBuffer(windowSwapIndex, colorInfo, depthInfo);
		inst._finalPass->submit(windowSwapIndex, waits, signal);
	}
	system::system(instance& vkinst, vector<string> textures, vector<string> models, vector<string> animations) : inst(vkinst) {
		texturePaths = textures;
		modelPaths = models;
		animationPaths = animations;
		uniformSize = 1024 * 1024 * 16;

		createFramebufferData();

		loadTextures();

		collectModelInfo();
		loadModels();
		initializeModelData();
		loadAnimations();

		stopped = false;
	}
	system::~system() {
		stopped = true;

		freeUniformBuffer();

		freeTextures();

		freeModels();

		freeFramebufferData();

		freeThreads();
	}
}