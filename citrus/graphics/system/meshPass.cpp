#include "citrus/graphics/system/meshPass.h"

namespace citrus::graphics {
    void meshPass::initializeDescriptors() {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo uboLayoutInfo = {};
		uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		uboLayoutInfo.bindingCount = 1;
		uboLayoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(sys.inst._device, &uboLayoutInfo, nullptr, &uboLayout) != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout");

		VkDescriptorPoolSize uboPoolSize = { };
		uboPoolSize.descriptorCount = SWAP_FRAMES;
		uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

		VkDescriptorPoolCreateInfo uboPoolInfo = { };
		uboPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		uboPoolInfo.poolSizeCount = 1;
		uboPoolInfo.maxSets = SWAP_FRAMES;
		uboPoolInfo.pPoolSizes = &uboPoolSize;

		if(vkCreateDescriptorPool(sys.inst._device, &uboPoolInfo, nullptr, &uboPool) != VK_SUCCESS) throw std::runtime_error("failed to create UBO descriptor pool");

		VkDescriptorSetLayout uboLayouts[SWAP_FRAMES] = { uboLayout, uboLayout };
		VkDescriptorSetAllocateInfo uboAllocInfo = { };
		uboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		uboAllocInfo.descriptorPool = uboPool;
		uboAllocInfo.descriptorSetCount = SWAP_FRAMES;
		uboAllocInfo.pSetLayouts = uboLayouts;

		vkAllocateDescriptorSets(sys.inst._device, &uboAllocInfo, uboSets);


		VkDescriptorSetLayoutBinding texLayoutBinding = {};
		texLayoutBinding.binding = 0;
		texLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texLayoutBinding.descriptorCount = (uint32_t)sys.textures.size();
		texLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo texLayoutInfo = {};
		texLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		texLayoutInfo.bindingCount = 1;
		texLayoutInfo.pBindings = &texLayoutBinding;

		if (vkCreateDescriptorSetLayout(sys.inst._device, &texLayoutInfo, nullptr, &texLayout) != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout");

		VkDescriptorPoolSize texPoolSize = { };
		texPoolSize.descriptorCount = (uint32_t) sys.textures.size();
		texPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		VkDescriptorPoolCreateInfo texPoolInfo = { };
		texPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		texPoolInfo.poolSizeCount = 1;
		texPoolInfo.maxSets = (uint32_t) sys.textures.size();
		texPoolInfo.pPoolSizes = &texPoolSize;

		vkCreateDescriptorPool(sys.inst._device, &texPoolInfo, nullptr, &texPool);

		VkDescriptorSetAllocateInfo texAllocInfo = { };
		texAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		texAllocInfo.descriptorPool = texPool;
		texAllocInfo.descriptorSetCount = 1;
		texAllocInfo.pSetLayouts = &texLayout;

		vkAllocateDescriptorSets(sys.inst._device, &texAllocInfo, &texSet);

		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			VkDescriptorBufferInfo uboBufInfo = { };
			uboBufInfo.buffer = sys.uniformBuffers[i];
			uboBufInfo.offset = 0;
			uboBufInfo.range = sys.uniformSize;

			VkWriteDescriptorSet uboWrite = { };
			uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uboWrite.descriptorCount = 1;
			uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			uboWrite.dstBinding = 0;
			uboWrite.dstSet = uboSets[i];
			uboWrite.pBufferInfo = &uboBufInfo;

			vkUpdateDescriptorSets(sys.inst._device, 1, &uboWrite, 0, nullptr);
		}

		vector<VkDescriptorImageInfo> imageInfos;
		for (uint32_t i = 0; i < sys.textures.size(); i++) {
			imageInfos.push_back({});
			imageInfos[i].sampler = sys.textures[i].samp;
			imageInfos[i].imageView = sys.textures[i].view;
			imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		VkWriteDescriptorSet texWrite = {};
		texWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		texWrite.descriptorCount = (uint32_t)sys.textures.size();
		texWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texWrite.dstBinding = 0;
		texWrite.dstSet = texSet;
		texWrite.pImageInfo = imageInfos.data();

		vkUpdateDescriptorSets(sys.inst._device, 1, &texWrite, 0, nullptr);
	}
    void meshPass::initializeRenderPass() {
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
		depthAttachment.format = sys.inst.findDepthFormat();
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

		if (vkCreateRenderPass(sys.inst._device, &renderPassInfo, nullptr, &pass) != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");
    }
	void meshPass::initializePipelineLayout() {
		VkPushConstantRange pcRange = { };
		pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pcRange.size = pcVertSize;
		pcRange.offset = 0;

		VkPushConstantRange frag_pcRange = { };
		frag_pcRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_pcRange.size = pcFragSize;
		frag_pcRange.offset = pcVertSize;

		VkPushConstantRange ranges[2] = { pcRange, frag_pcRange };

		VkDescriptorSetLayout setLayouts[2] = { uboLayout, texLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 2;
		pipelineLayoutInfo.pPushConstantRanges = ranges;
		pipelineLayoutInfo.setLayoutCount = 2;
		pipelineLayoutInfo.pSetLayouts = setLayouts;

		if (vkCreatePipelineLayout(sys.inst._device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout");
	}
	void meshPass::initializePipeline() {

		VkShaderModule vertModule, fragModule;
		VkPipelineShaderStageCreateInfo vertInfo = { }, fragInfo = { };
		{
			string src = util::loadEntireFile(vert.string());

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
			string src = util::loadEntireFile(frag.string());

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
		
		VkPipelineShaderStageCreateInfo stages[] = { vertInfo, fragInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)meshMappings.usages.size();
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t) meshMappings.usages.size();
		vertexInputInfo.pVertexBindingDescriptions = meshMappings.bindings.data();
		vertexInputInfo.pVertexAttributeDescriptions = meshMappings.attribs.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)sys.inst.width;
		viewport.height = (float)sys.inst.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { sys.inst.width, sys.inst.height };

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
		pipelineInfo.pDepthStencilState = &depthStateInfo;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = pass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(sys.inst._device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");

		vkDestroyShaderModule(sys.inst._device, vertInfo.module, nullptr);
		vkDestroyShaderModule(sys.inst._device, fragInfo.module, nullptr);
	}
	void meshPass::initializeFramebuffers() {
		VkImageCreateInfo colorInfo = {};
		colorInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colorInfo.imageType = VK_IMAGE_TYPE_2D;
		colorInfo.extent.width = sys.inst.width;
		colorInfo.extent.height = sys.inst.height;
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
		depthInfo.extent.width = sys.inst.width;
		depthInfo.extent.height = sys.inst.height;
		depthInfo.extent.depth = 1;
		depthInfo.mipLevels = 1;
		depthInfo.arrayLayers = 1;
		depthInfo.format = sys.inst.findDepthFormat();
		depthInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		depthInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		depthInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateImage(sys.inst._device, &colorInfo, nullptr, &frames[i].color) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			if (vkCreateImage(sys.inst._device, &depthInfo, nullptr, &frames[i].depth) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements colorRequirements, depthRequirements;
			vkGetImageMemoryRequirements(sys.inst._device, frames[i].color, &colorRequirements);
			vkGetImageMemoryRequirements(sys.inst._device, frames[i].depth, &depthRequirements);

			VkMemoryAllocateInfo colorAllocInfo = {}, depthAllocInfo = {};
			colorAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			colorAllocInfo.memoryTypeIndex = sys.inst.findMemoryType(colorRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			colorAllocInfo.allocationSize = colorRequirements.size;
			depthAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			depthAllocInfo.memoryTypeIndex = sys.inst.findMemoryType(depthRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			depthAllocInfo.allocationSize = depthRequirements.size;
			vkAllocateMemory(sys.inst._device, &colorAllocInfo, nullptr, &frames[i].colorMem);
			vkAllocateMemory(sys.inst._device, &depthAllocInfo, nullptr, &frames[i].depthMem);

			vkBindImageMemory(sys.inst._device, frames[i].color, frames[i].colorMem, 0);
			vkBindImageMemory(sys.inst._device, frames[i].depth, frames[i].depthMem, 0);

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
			depthViewInfo.format = sys.inst.findDepthFormat();
			depthViewInfo.image = frames[i].depth;
			depthViewInfo.subresourceRange.baseMipLevel = 0;
			depthViewInfo.subresourceRange.baseArrayLayer = 0;
			depthViewInfo.subresourceRange.levelCount = 1;
			depthViewInfo.subresourceRange.layerCount = 1;
			depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			vkCreateImageView(sys.inst._device, &colorViewInfo, nullptr, &frames[i].colorView);
			vkCreateImageView(sys.inst._device, &depthViewInfo, nullptr, &frames[i].depthView);

			VkSamplerCreateInfo colorSampInfo = {};
			colorSampInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			colorSampInfo.magFilter = VK_FILTER_NEAREST;
			colorSampInfo.minFilter = VK_FILTER_NEAREST;
			colorSampInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			colorSampInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			colorSampInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			colorSampInfo.anisotropyEnable = VK_FALSE;
			colorSampInfo.maxAnisotropy = 1;
			colorSampInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			colorSampInfo.unnormalizedCoordinates = VK_FALSE;
			colorSampInfo.compareEnable = VK_FALSE;
			colorSampInfo.compareOp = VK_COMPARE_OP_NEVER;
			colorSampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			colorSampInfo.mipLodBias = 0.0f;
			colorSampInfo.minLod = 0.0f;
			colorSampInfo.maxLod = 0.0f;

			VkSamplerCreateInfo depthSampInfo = {};
			depthSampInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			depthSampInfo.magFilter = VK_FILTER_NEAREST;
			depthSampInfo.minFilter = VK_FILTER_NEAREST;
			depthSampInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			depthSampInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			depthSampInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			depthSampInfo.anisotropyEnable = VK_FALSE;
			depthSampInfo.maxAnisotropy = 1;
			depthSampInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			depthSampInfo.unnormalizedCoordinates = VK_FALSE;
			depthSampInfo.compareEnable = VK_FALSE;
			depthSampInfo.compareOp = VK_COMPARE_OP_NEVER;
			depthSampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			depthSampInfo.mipLodBias = 0.0f;
			depthSampInfo.minLod = 0.0f;
			depthSampInfo.maxLod = 0.0f;

			vkCreateSampler(sys.inst._device, &colorSampInfo, nullptr, &frames[i].colorSamp);
			vkCreateSampler(sys.inst._device, &depthSampInfo, nullptr, &frames[i].depthSamp);

			VkImageView views[2] = { frames[i].colorView, frames[i].depthView };

			VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.width = sys.inst.width;
			fbInfo.height = sys.inst.height;
			fbInfo.layers = 1;
			fbInfo.renderPass = pass;
			fbInfo.attachmentCount = 2;
			fbInfo.pAttachments = views;

			if(vkCreateFramebuffer(sys.inst._device, &fbInfo, nullptr, &frames[i].fbo) != VK_SUCCESS) throw std::runtime_error("couldn't create meshPass FBO");
		}
	}
    
	void meshPass::mapModels() {
		// check if attribs are supported
		uint32_t reqUse = 0;
		for (int i = 0; i < requiredUsages.size(); i++) {
			reqUse |= (uint32_t)requiredUsages[i];
		}
		for(int i = 0; i < sys.models.size(); i++) {
			uint32_t modelUse = (uint32_t)sys.models[i].desc.allUsage;
			if((modelUse & reqUse) != reqUse) continue;

			mappings.push_back({ i, meshMappings.makePartialStructureView(sys.models[i].desc) });
		}
		util::sout("Mesh Pass:\n");
		util::sout("  Mapped Models: " + std::to_string(mappings.size()) + "\n");
		for (int i = 0; i < mappings.size(); i++) {
			util::sout("    " + std::to_string(i) + ": " + sys.models[mappings[i].modelIndex].source.relative_path().string() + "\n");
		}
	}

	VkDescriptorImageInfo meshPass::getColorInfo() {
		VkDescriptorImageInfo colorInfo = { };
		colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorInfo.imageView = frames[sys.frameIndex].colorView;
		colorInfo.sampler = frames[sys.frameIndex].colorSamp;
		return colorInfo;
	}

	VkDescriptorImageInfo meshPass::getDepthInfo() {
		VkDescriptorImageInfo depthInfo = { };
		depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthInfo.imageView = frames[sys.frameIndex].depthView;
		depthInfo.sampler = frames[sys.frameIndex].depthSamp;
		return depthInfo;
	}

	void meshPass::preRender(uint32_t const & threadCount) {

		sys.inst.waitForFence(waitFences[sys.frameIndex]);
		sys.inst.resetFence(waitFences[sys.frameIndex]);

		ranges.resize(threadCount);
		ranges[0].begin = 0;
		for (uint32_t i = 1; i < threadCount; i++) {
			ranges[i - 1].end = uint32_t(i * float(items.size()) / float(threadCount));
			ranges[i].begin = ranges[i - 1].end;
		}
		ranges[threadCount - 1].end = items.size();
	}

	void meshPass::renderPartial(uint32_t const & threadIndex) {
		//util::sout("renderPartial " + std::to_string(threadIndex));
		int vbinds = 0, draws = 0, culls = 0;
		hptime partialStart = hpclock::now();

		VkCommandBuffer& buf = secBufs[sys.frameIndex][threadIndex];
		{	std::lock_guard<std::mutex> lock(sys.instMut);
			if(buf != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(buf, sys.commandPools[threadIndex]);
			buf = sys.inst.createCommandBuffer(sys.commandPools[threadIndex], true);
		}

		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		cmdInfo.pInheritanceInfo = &inheritanceInfos[sys.frameIndex];

		vkBeginCommandBuffer(buf, &cmdInfo);

		vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &texSet, 0, nullptr);

		uint32_t lastModelBindIndex = 999999;
		for(uint32_t i = ranges[threadIndex].begin; i < ranges[threadIndex].end; i++) {
			if (!cullObscured || sys.frameCull.testSphere(items[i].pos, sys.models[mappings[items[i].modelIndex].modelIndex].radius)) {
				if (lastModelBindIndex != items[i].modelIndex) {
					vkCmdBindVertexBuffers(buf, 0, meshMappings.usages.size(), sys.vertexBuffers, mappings[items[i].modelIndex].desc.offsets.data());
					vkCmdBindIndexBuffer(buf, sys.indexBuffer, mappings[items[i].modelIndex].desc.indexOffset, VK_INDEX_TYPE_UINT16);
					vbinds++;
					lastModelBindIndex = items[i].modelIndex;
				}

				pcData pushData;
				mat4 modTmp = glm::translate(items[i].pos);
				pushData.model = modTmp;
				pushData.mvp = sys.frameVP * modTmp;
				pushData.uints[0] = items[i].texIndex;
				vkCmdPushConstants(buf, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pushData);
				vkCmdPushConstants(buf, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, pcVertSize, pcFragSize, &pushData.uints);
				uint32_t zero = 0;
				vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &uboSets[sys.frameIndex], 1, &zero);

				vkCmdDrawIndexed(buf, mappings[items[i].modelIndex].desc.indexCount, 1, 0, 0, 0);
				draws++;
			} else {
				culls++;
			}
		}

		vkEndCommandBuffer(buf);
	}

	void meshPass::postRender(uint32_t const& threadCount) {
		//util::sout("postRender\n");
		if (priBufs[sys.frameIndex] != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(priBufs[sys.frameIndex], sys.inst._commandPool);
		priBufs[sys.frameIndex] = sys.inst.createCommandBuffer(sys.inst._commandPool, false);

		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(priBufs[sys.frameIndex], &beginInfo);

		VkClearValue clearValues[2] = { };
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassInfo = { };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pass;
		renderPassInfo.framebuffer = frames[sys.frameIndex].fbo;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = sys.inst._extent;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(priBufs[sys.frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		vkCmdExecuteCommands(priBufs[sys.frameIndex], threadCount, secBufs[sys.frameIndex].data());

		vkCmdEndRenderPass(priBufs[sys.frameIndex]);

		vkEndCommandBuffer(priBufs[sys.frameIndex]);

		VkSubmitInfo subInfo = { };
		subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInfo.commandBufferCount = 1;
		subInfo.pCommandBuffers = &priBufs[sys.frameIndex];
		subInfo.signalSemaphoreCount = 1;
		VkSemaphore signalSem = getSignalSem();
		subInfo.pSignalSemaphores = &signalSem;

		vkQueueSubmit(sys.inst._graphicsQueue, 1, &subInfo, waitFences[sys.frameIndex]);
	}
	
	meshPass::meshPass(system & sys, fpath const& vertLoc, fpath const& fragLoc) : sysNode(sys) {
		sys.meshPasses.push_back(this);

		vert = vertLoc;
		frag = fragLoc;

		map<meshAttributeUsage, uint32_t> locMap;
		locMap[meshAttributeUsage::positionType] = 0;
		locMap[meshAttributeUsage::normalType] = 1;
		locMap[meshAttributeUsage::uvType] = 2;
		meshMappings = meshUsageLocationMapping(locMap);
		for (auto const& kvp : locMap) {
			requiredUsages.push_back(kvp.first);
		}

		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			waitFences[i] = sys.inst.createFence(true);
			priBufs[i] = sys.inst.createCommandBuffer(sys.inst._commandPool);
			secBufs[i].resize(sys.renderThreads.size());
		}

		initializeDescriptors();
		initializeRenderPass();
		initializePipelineLayout();
		initializePipeline();
		initializeFramebuffers();

		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			inheritanceInfos[i] = { };
			inheritanceInfos[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfos[i].framebuffer = frames[i].fbo;
			inheritanceInfos[i].renderPass = pass;
		}

		mapModels();
	}
    meshPass::~meshPass() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			vkDestroyFramebuffer(sys.inst._device, frames[i].fbo, nullptr);
			vkDestroySampler(sys.inst._device, frames[i].depthSamp, nullptr);
			vkDestroySampler(sys.inst._device, frames[i].colorSamp, nullptr);
			vkDestroyImageView(sys.inst._device, frames[i].colorView, nullptr);
			vkDestroyImageView(sys.inst._device, frames[i].depthView, nullptr);
			vkDestroyImage(sys.inst._device, frames[i].color, nullptr);
			vkDestroyImage(sys.inst._device, frames[i].depth, nullptr);
			vkFreeMemory(sys.inst._device, frames[i].colorMem, nullptr);
			vkFreeMemory(sys.inst._device, frames[i].depthMem, nullptr);
		}
    }
}