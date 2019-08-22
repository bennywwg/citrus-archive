#include "citrus/graphics/system/meshPass.h"

namespace citrus::graphics {
	void meshPass::initializeDescriptors() {
		{
			VkDescriptorSetLayoutBinding uboLayoutBinding = { };
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo uboLayoutInfo = { };
			uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			uboLayoutInfo.bindingCount = 1;
			uboLayoutInfo.pBindings = &uboLayoutBinding;

			if (vkCreateDescriptorSetLayout(sys.inst._device, &uboLayoutInfo, nullptr, &uboLayout) != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout");

			VkDescriptorPoolSize uboPoolSize = { };
			uboPoolSize.descriptorCount = SWAP_FRAMES;
			uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			VkDescriptorPoolCreateInfo uboPoolInfo = { };
			uboPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			uboPoolInfo.poolSizeCount = 1;
			uboPoolInfo.maxSets = SWAP_FRAMES;
			uboPoolInfo.pPoolSizes = &uboPoolSize;

			if (vkCreateDescriptorPool(sys.inst._device, &uboPoolInfo, nullptr, &uboPool) != VK_SUCCESS) throw std::runtime_error("failed to create UBO descriptor pool");

			VkDescriptorSetLayout uboLayouts[SWAP_FRAMES] = { uboLayout, uboLayout };
			VkDescriptorSetAllocateInfo uboAllocInfo = { };
			uboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			uboAllocInfo.descriptorPool = uboPool;
			uboAllocInfo.descriptorSetCount = SWAP_FRAMES;
			uboAllocInfo.pSetLayouts = uboLayouts;

			vkAllocateDescriptorSets(sys.inst._device, &uboAllocInfo, uboSets);
		}

		{
			VkDescriptorSetLayoutBinding ssboLayoutBinding = { };
			ssboLayoutBinding.binding = 0;
			ssboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			ssboLayoutBinding.descriptorCount = 1;
			ssboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutCreateInfo ssboLayoutInfo = { };
			ssboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			ssboLayoutInfo.bindingCount = 1;
			ssboLayoutInfo.pBindings = &ssboLayoutBinding;

			if (vkCreateDescriptorSetLayout(sys.inst._device, &ssboLayoutInfo, nullptr, &ssboLayout) != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout");

			VkDescriptorPoolSize ssboPoolSize = { };
			ssboPoolSize.descriptorCount = SWAP_FRAMES;
			ssboPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

			VkDescriptorPoolCreateInfo ssboPoolInfo = { };
			ssboPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			ssboPoolInfo.poolSizeCount = 1;
			ssboPoolInfo.maxSets = SWAP_FRAMES;
			ssboPoolInfo.pPoolSizes = &ssboPoolSize;

			if (vkCreateDescriptorPool(sys.inst._device, &ssboPoolInfo, nullptr, &ssboPool) != VK_SUCCESS) throw std::runtime_error("failed to create UBO descriptor pool");

			VkDescriptorSetLayout ssboLayouts[SWAP_FRAMES] = { ssboLayout, ssboLayout };
			VkDescriptorSetAllocateInfo ssboAllocInfo = { };
			ssboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			ssboAllocInfo.descriptorPool = ssboPool;
			ssboAllocInfo.descriptorSetCount = SWAP_FRAMES;
			ssboAllocInfo.pSetLayouts = ssboLayouts;

			vkAllocateDescriptorSets(sys.inst._device, &ssboAllocInfo, ssboSets);
		}

		{
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
			texPoolSize.descriptorCount = (uint32_t)sys.textures.size();
			texPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			VkDescriptorPoolCreateInfo texPoolInfo = { };
			texPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			texPoolInfo.poolSizeCount = 1;
			texPoolInfo.maxSets = (uint32_t)sys.textures.size();
			texPoolInfo.pPoolSizes = &texPoolSize;

			vkCreateDescriptorPool(sys.inst._device, &texPoolInfo, nullptr, &texPool);

			VkDescriptorSetAllocateInfo texAllocInfo = { };
			texAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			texAllocInfo.descriptorPool = texPool;
			texAllocInfo.descriptorSetCount = 1;
			texAllocInfo.pSetLayouts = &texLayout;

			vkAllocateDescriptorSets(sys.inst._device, &texAllocInfo, &texSet);
		}

		{
			VkDescriptorSetLayoutBinding cubeLayoutBinding = {};
			cubeLayoutBinding.binding = 0;
			cubeLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			cubeLayoutBinding.descriptorCount = (uint32_t)sys.cubemaps.cubemaps.size();
			cubeLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo cubeLayoutInfo = {};
			cubeLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			cubeLayoutInfo.bindingCount = 1;
			cubeLayoutInfo.pBindings = &cubeLayoutBinding;

			if (vkCreateDescriptorSetLayout(sys.inst._device, &cubeLayoutInfo, nullptr, &cubeLayout) != VK_SUCCESS) throw std::runtime_error("failed to create cubemap descriptor set layout");

			VkDescriptorPoolSize cubePoolSize = { };
			cubePoolSize.descriptorCount = (uint32_t)sys.cubemaps.cubemaps.size();
			cubePoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			VkDescriptorPoolCreateInfo cubePoolInfo = { };
			cubePoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			cubePoolInfo.poolSizeCount = 1;
			cubePoolInfo.maxSets = (uint32_t)sys.cubemaps.cubemaps.size();
			cubePoolInfo.pPoolSizes = &cubePoolSize;

			vkCreateDescriptorPool(sys.inst._device, &cubePoolInfo, nullptr, &cubePool);

			VkDescriptorSetAllocateInfo cubeAllocInfo = { };
			cubeAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			cubeAllocInfo.descriptorPool = cubePool;
			cubeAllocInfo.descriptorSetCount = 1;
			cubeAllocInfo.pSetLayouts = &cubeLayout;

			vkAllocateDescriptorSets(sys.inst._device, &cubeAllocInfo, &cubeSet);
		}

		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			VkDescriptorBufferInfo uboBufInfo = { };
			uboBufInfo.buffer = ubos[i].buf;
			uboBufInfo.offset = 0;
			uboBufInfo.range = uboSize;

			VkWriteDescriptorSet uboWrite = { };
			uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uboWrite.descriptorCount = 1;
			uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboWrite.dstBinding = 0;
			uboWrite.dstSet = uboSets[i];
			uboWrite.pBufferInfo = &uboBufInfo;

			vkUpdateDescriptorSets(sys.inst._device, 1, &uboWrite, 0, nullptr);
		}
		if (rigged) {
			for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
				VkDescriptorBufferInfo ssboBufInfo = { };
				ssboBufInfo.buffer = ssbos[i].buf;
				ssboBufInfo.offset = 0;
				ssboBufInfo.range = sizeof(mat4) * 64;

				VkWriteDescriptorSet ssboWrite = { };
				ssboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				ssboWrite.descriptorCount = 1;
				ssboWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				ssboWrite.dstBinding = 0;
				ssboWrite.dstSet = ssboSets[i];
				ssboWrite.pBufferInfo = &ssboBufInfo;

				vkUpdateDescriptorSets(sys.inst._device, 1, &ssboWrite, 0, nullptr);
			}
		}
		
		{
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

		{
			vector<VkDescriptorImageInfo> cubeInfos;
			for (uint32_t i = 0; i < sys.cubemaps.cubemaps.size(); i++) {
				cubeInfos.push_back({});
				cubeInfos[i].sampler = sys.cubemaps.cubemaps[i].samp;
				cubeInfos[i].imageView = sys.cubemaps.cubemaps[i].view;
				cubeInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}

			VkWriteDescriptorSet cubeWrite = {};
			cubeWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			cubeWrite.descriptorCount = (uint32_t)sys.cubemaps.cubemaps.size();
			cubeWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			cubeWrite.dstBinding = 0;
			cubeWrite.dstSet = cubeSet;
			cubeWrite.pImageInfo = cubeInfos.data();

			vkUpdateDescriptorSets(sys.inst._device, 1, &cubeWrite, 0, nullptr);
		}
	}
    void meshPass::initializeRenderPass() {
		vector<VkAttachmentDescription> attachments = frame->getAttachmentDescriptions(transitionToRead);
		vector<VkAttachmentReference> colorRefs = frame->getColorAttachmentRefs();
		VkAttachmentReference depthRef = frame->getDepthAttachmentRef();

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = colorRefs.size();
		subpass.pColorAttachments = colorRefs.data();
		subpass.pDepthStencilAttachment = &depthRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
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

		vector<VkDescriptorSetLayout> setLayouts;
		if (!rigged) {
			setLayouts = { uboLayout, texLayout, cubeLayout };
		} else {
			setLayouts = { uboLayout, texLayout, cubeLayout, ssboLayout };
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 2;
		pipelineLayoutInfo.pPushConstantRanges = ranges;
		pipelineLayoutInfo.setLayoutCount = (uint32_t) setLayouts.size();
		pipelineLayoutInfo.pSetLayouts = setLayouts.data();

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

		VkPipelineColorBlendAttachmentState states[2] = { colorBlendAttachment, colorBlendAttachment };

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 2;
		colorBlending.pAttachments = states;
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
		for (int i = 0; i < SWAP_FRAMES; i++) {
			vector<VkImageView> views = frame->getViews(i);

			VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.width = sys.inst.width;
			fbInfo.height = sys.inst.height;
			fbInfo.layers = 1;
			fbInfo.renderPass = pass;
			fbInfo.attachmentCount = views.size();
			fbInfo.pAttachments = views.data();

			if (vkCreateFramebuffer(sys.inst._device, &fbInfo, nullptr, &fbos[i]) != VK_SUCCESS) throw std::runtime_error("couldn't create meshPass FBO");
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
		util::sout("Mesh Pass (");
		util::sout(rigged ? "rigged " : "");
		util::sout("):\n");
		util::sout("  Mapped Models: " + std::to_string(mappings.size()) + "\n");
		for (int i = 0; i < mappings.size(); i++) {
			util::sout("    " + std::to_string(i) + ": " + sys.models[mappings[i].modelIndex].source.relative_path().string() + "\n");
		}
	}

	void meshPass::preRender(uint32_t const & threadCount) {
		sys.inst.waitForFence(waitFences[sys.frameIndex]);
		sys.inst.resetFence(waitFences[sys.frameIndex]);
		
		uniformBlock& uniData = *(uniformBlock*)ubos[sys.frameIndex].mapped;
		uniData.lightCount = 1;
		uniData.lightDirs[0] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		uniData.lightDirs[1] = vec4(0.0f, 4.0f, 0.0f, 1.0f);
		uniData.camPos = vec4(sys.frameCam.pos, 1.0f);
		ubos[sys.frameIndex].flushRange(0, sizeof(uniformBlock));

		ranges.resize(threadCount);
		ranges[0].begin = 0;
		for (uint32_t i = 1; i < threadCount; i++) {
			ranges[i - 1].end = uint32_t(i * float(items.size()) / float(threadCount));
			ranges[i].begin = ranges[i - 1].end;
		}
		ranges[threadCount - 1].end = items.size();
	}

	void meshPass::renderPartial(uint32_t const & threadIndex) {
		hptime partialStart = hpclock::now();

		// computes animation transform data for each item belonging to this thread
		// where each item (starting at item index ranges[threadIndex].begin) has bone data
		// number of bones per item shouldn't be needed later
		vector<uint64_t> boneOffsets;
		if (threadIndex == 3)
			boneOffsets = { };
		if (rigged) {
			uint64_t availableForMyThread = ssbos[sys.frameIndex].size / sys.renderThreads.size();
			boneOffsets.resize(ranges[threadIndex].end - ranges[threadIndex].begin);
			uint64_t boneOffset = util::roundUpAlign(availableForMyThread * (uint64_t) threadIndex, ssbos[sys.frameIndex].align);
			ranges[threadIndex].offset = boneOffset;
			//todo: don't do this if culled
			for (uint32_t i = ranges[threadIndex].begin; i < ranges[threadIndex].end; i++) {
				mesh& m = sys.models[mappings[items[i].modelIndex].modelIndex].m;
				vector<mat4> res;
				m.calculateAnimationTransforms(items[i].animationIndex, res, items[i].aniTime, behavior::repeat);
				if ((boneOffset + res.size() * sizeof(mat4)) > ssbos[sys.frameIndex].size) throw std::runtime_error("ran out of bones");
				memcpy(stagings[sys.frameIndex].mapped + boneOffset, res.data(), sizeof(mat4) * res.size());
				boneOffsets[i - ranges[threadIndex].begin] = boneOffset;
				boneOffset += res.size() * sizeof(mat4);
				boneOffset = util::roundUpAlign(boneOffset, ssbos[sys.frameIndex].align);
			}
			ranges[threadIndex].size = boneOffset - ranges[threadIndex].offset;
		}

		//util::sout("renderPartial " + std::to_string(threadIndex));
		int vbinds = 0, draws = 0, culls = 0;

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

		vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &uboSets[sys.frameIndex], 0, nullptr);
		vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &texSet, 0, nullptr);
		vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 2, 1, &cubeSet, 0, nullptr);

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
				glm::mat4x3 modTmp = glm::translate(items[i].pos) * glm::toMat4(items[i].ori);
				util::copyMat4x3ToRowMajor(modTmp, pushData.rowMajorModel);
				pushData.mvp = sys.frameVP * mat4(modTmp);
				pushData.uints[0] = items[i].texIndex;
				pushData.uints[1] = i + initialIndex;
				pushData.uints[2] = items[i].normalTexIndex;
				vkCmdPushConstants(buf, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, pcVertSize, &pushData);
				vkCmdPushConstants(buf, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, pcVertSize, pcFragSize, &pushData.uints);
				
				if (rigged) {
					uint32_t val = boneOffsets[i - ranges[threadIndex].begin];
					vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3, 1, &ssboSets[sys.frameIndex], 1, &val);
				}

				vkCmdDrawIndexed(buf, mappings[items[i].modelIndex].desc.indexCount, 1, 0, 0, 0);
				draws++;
			} else {
				culls++;
			}
		}

		vkEndCommandBuffer(buf);
	}

	void meshPass::postRender(uint32_t const& threadCount) {
		if (rigged) {
			for (int i = 0; i < threadCount; i++) {
				stagings[sys.frameIndex].flushRange(ranges[i].offset, ranges[i].size);
			}
			
			VkBufferCopy reg = { };
			reg.srcOffset = 0;
			reg.dstOffset = 0;
			reg.size = stagings[sys.frameIndex].size;

			if (stagingCommands[sys.frameIndex] != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(stagingCommands[sys.frameIndex], sys.inst._commandPool);
			stagingCommands[sys.frameIndex] = sys.inst.createCommandBuffer(sys.inst._commandPool);
			
			VkCommandBufferBeginInfo inf = { };
			inf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			inf.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(stagingCommands[sys.frameIndex], &inf);
			vkCmdCopyBuffer(stagingCommands[sys.frameIndex], stagings[sys.frameIndex].buf, ssbos[sys.frameIndex].buf, 1, &reg);
			vkEndCommandBuffer(stagingCommands[sys.frameIndex]);

			VkSubmitInfo subInf = { };
			subInf.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			subInf.signalSemaphoreCount = 1;
			subInf.pSignalSemaphores = &stagingSems[sys.frameIndex];
			subInf.commandBufferCount = 1;
			subInf.pCommandBuffers = &stagingCommands[sys.frameIndex];
			vkQueueSubmit(sys.inst._graphicsQueue, 1, &subInf, VK_NULL_HANDLE);
		}

		//util::sout("postRender\n");
		if (priBufs[sys.frameIndex] != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(priBufs[sys.frameIndex], sys.inst._commandPool);
		priBufs[sys.frameIndex] = sys.inst.createCommandBuffer(sys.inst._commandPool, false);

		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(priBufs[sys.frameIndex], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = { };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pass;
		renderPassInfo.framebuffer = fbos[sys.frameIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = sys.inst._extent;

		vkCmdBeginRenderPass(priBufs[sys.frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		vkCmdExecuteCommands(priBufs[sys.frameIndex], threadCount, secBufs[sys.frameIndex].data());

		vkCmdEndRenderPass(priBufs[sys.frameIndex]);

		vkEndCommandBuffer(priBufs[sys.frameIndex]);

		VkSubmitInfo subInfo = { };
		subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInfo.commandBufferCount = 1;
		subInfo.pCommandBuffers = &priBufs[sys.frameIndex];
		vector<VkSemaphore> signalSems = getSignalSems();
		subInfo.signalSemaphoreCount = signalSems.size();
		subInfo.pSignalSemaphores = signalSems.data();
		vector<VkSemaphore> waitSems = getWaitSems();
		vector<VkPipelineStageFlags> waitFlags(waitSems.size(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		if (rigged) {
			waitSems.push_back(stagingSems[sys.frameIndex]);
			waitFlags.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);
		}
		subInfo.waitSemaphoreCount = waitSems.size();
		subInfo.pWaitSemaphores = waitSems.data();
		subInfo.pWaitDstStageMask = waitFlags.data();

		vkQueueSubmit(sys.inst._graphicsQueue, 1, &subInfo, waitFences[sys.frameIndex]);
	}
	
	meshPass::meshPass(system & sys, frameStore* fstore, bool textured, bool lit, bool rigged, fpath const& vertLoc, fpath const& fragLoc, bool transitionToRead) :
		sysNode(sys),
		frame(fstore) {
		sys.meshPasses.push_back(this);

		this->rigged = rigged;

		vert = vertLoc;
		frag = fragLoc;

		this->transitionToRead = transitionToRead;

		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			ubos[i].init(&sys.inst, uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
		}

		map<meshAttributeUsage, uint32_t> locMap;
		locMap[meshAttributeUsage::positionType] = 0;
		if(lit) locMap[meshAttributeUsage::normalType] = 1;
		if(textured) locMap[meshAttributeUsage::uvType] = 2;
		if (lit && textured) {
			locMap[meshAttributeUsage::tangentType] = 3;
			locMap[meshAttributeUsage::bitangentType] = 4;
		}
		if (rigged) {
			locMap[meshAttributeUsage::bone0Type] = 5;
			locMap[meshAttributeUsage::bone1Type] = 6;
			locMap[meshAttributeUsage::weight0Type] = 7;
			locMap[meshAttributeUsage::weight1Type] = 8;

			for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
				ssbos[i].init(&sys.inst, ssboSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
				stagings[i].init(&sys.inst, ssboSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
			}
		}
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
			inheritanceInfos[i].framebuffer = fbos[i];
			inheritanceInfos[i].renderPass = pass;
			stagingCommands[i] = VK_NULL_HANDLE;
			stagingSems[i] = sys.inst.createSemaphore();
		}

		mapModels();
	}
    meshPass::~meshPass() {
		vkDestroyDescriptorPool(sys.inst._device, uboPool, nullptr);
		vkDestroyDescriptorPool(sys.inst._device, texPool, nullptr);
		vkDestroyDescriptorPool(sys.inst._device, ssboPool, nullptr);
		vkDestroyDescriptorPool(sys.inst._device, cubePool, nullptr);
		vkDestroyDescriptorSetLayout(sys.inst._device, uboLayout, nullptr);
		vkDestroyDescriptorSetLayout(sys.inst._device, texLayout, nullptr);
		vkDestroyDescriptorSetLayout(sys.inst._device, ssboLayout, nullptr);
		vkDestroyDescriptorSetLayout(sys.inst._device, cubeLayout, nullptr);
		vkDestroyPipeline(sys.inst._device, pipeline, nullptr);
		vkDestroyPipelineLayout(sys.inst._device, pipelineLayout, nullptr);
		vkDestroyRenderPass(sys.inst._device, pass, nullptr);
		for (int i = 0; i < SWAP_FRAMES; i++) {
			vkDestroyFence(sys.inst._device, waitFences[i], nullptr);
			vkDestroyFramebuffer(sys.inst._device, fbos[i], nullptr);
			sys.inst.destroySemaphore(stagingSems[i]);
		}
    }
}