#include <citrus/graphics/system/immediatePass.h>

namespace citrus::graphics {
	void immediatePass::initializeDescriptors() {
		{
			VkDescriptorSetLayoutBinding uboLayoutBinding = { };
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo uboLayoutInfo = { };
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

			if (vkCreateDescriptorPool(sys.inst._device, &uboPoolInfo, nullptr, &uboPool) != VK_SUCCESS) throw std::runtime_error("failed to create UBO descriptor pool");

			VkDescriptorSetLayout uboLayouts[SWAP_FRAMES] = { uboLayout, uboLayout };
			VkDescriptorSetAllocateInfo uboAllocInfo = { };
			uboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			uboAllocInfo.descriptorPool = uboPool;
			uboAllocInfo.descriptorSetCount = SWAP_FRAMES;
			uboAllocInfo.pSetLayouts = uboLayouts;

			vkAllocateDescriptorSets(sys.inst._device, &uboAllocInfo, uboSets);
		}

		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			VkDescriptorBufferInfo uboBufInfo = { };
			uboBufInfo.buffer = ubos[i].buf;
			uboBufInfo.offset = 0;
			uboBufInfo.range = sizeof(uniformBlock);

			VkWriteDescriptorSet uboWrite = { };
			uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uboWrite.descriptorCount = 1;
			uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			uboWrite.dstBinding = 0;
			uboWrite.dstSet = uboSets[i];
			uboWrite.pBufferInfo = &uboBufInfo;

			vkUpdateDescriptorSets(sys.inst._device, 1, &uboWrite, 0, nullptr);
		}
	}
	void immediatePass::initializeRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.finalLayout = transitionToRead ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription indexAttachment = {};
		indexAttachment.format = VK_FORMAT_R16_UINT;
		indexAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		indexAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		indexAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		indexAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		indexAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		indexAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		indexAttachment.finalLayout = transitionToRead ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = sys.inst.findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = transitionToRead ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription attachments[] = { colorAttachment, indexAttachment, depthAttachment };

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference indexAttachmentRef = {};
		indexAttachmentRef.attachment = 1;
		indexAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorRefs[2] = { colorAttachmentRef, indexAttachmentRef };

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 2;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 2;
		subpass.pColorAttachments = colorRefs;
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
		renderPassInfo.attachmentCount = 3;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(sys.inst._device, &renderPassInfo, nullptr, &pass) != VK_SUCCESS) throw std::runtime_error("failed to create render pass!");
	}
	void immediatePass::initializePipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &uboLayout;

		if (vkCreatePipelineLayout(sys.inst._device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout");
	}
	void immediatePass::initializePipeline() {

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

		VkVertexInputBindingDescription bindingDesc = { };
		bindingDesc.binding = 0;
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDesc.stride = sizeof(vec3);

		VkVertexInputAttributeDescription attribDesc = { };
		attribDesc.binding = 0;
		attribDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribDesc.offset = 0;
		attribDesc.location = 0;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
		vertexInputInfo.pVertexAttributeDescriptions = &attribDesc;

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

		VkPipelineColorBlendAttachmentState states[3] = { colorBlendAttachment, colorBlendAttachment, colorBlendAttachment };

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
		depthStateInfo.depthWriteEnable = VK_FALSE;
		depthStateInfo.depthTestEnable = VK_FALSE;
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
	void immediatePass::initializeFramebuffers() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			VkImageView views[3] = { frame->frames[i].color.view, frame->frames[i].index.view, frame->frames[i].depth.view, };

			VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.width = sys.inst.width;
			fbInfo.height = sys.inst.height;
			fbInfo.layers = 1;
			fbInfo.renderPass = pass;
			fbInfo.attachmentCount = 3;
			fbInfo.pAttachments = views;

			if (vkCreateFramebuffer(sys.inst._device, &fbInfo, nullptr, &fbos[i]) != VK_SUCCESS) throw std::runtime_error("couldn't create meshPass FBO");
		}
	}
	void immediatePass::preRender(uint32_t const& threadCount) {
		sys.inst.waitForFence(waitFences[sys.frameIndex]);
		sys.inst.resetFence(waitFences[sys.frameIndex]);

		VkCommandBuffer& buf = priBufs[sys.frameIndex];
		if (buf != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(buf, sys.inst._commandPool);
		buf = sys.inst.createCommandBuffer(sys.inst._commandPool);

		vector<uint64_t> vertOffsets;
		vector<uint32_t> uniformOffsets;
		if (active) {
			uint64_t currentOffset = 0;
			uint64_t currentUniformOffset = 0;
			for (int i = 0; i < groupings.size(); i++) {
				memcpy(verts[sys.frameIndex].mapped + currentOffset, groupings[i].data.data(), groupings[i].data.size() * sizeof(vec3));
				vertOffsets.push_back(currentOffset);
				currentOffset = util::roundUpAlign(currentOffset + groupings[i].data.size() * sizeof(vec3), verts[sys.frameIndex].align);

				uniformBlock bk = { };
				bk.color = vec4(groupings[i].color, 1.0f);
				bk.mvp = sys.frameVP * groupings[i].tr;
				memcpy(ubos[sys.frameIndex].mapped + currentUniformOffset, &bk, sizeof(uniformBlock));
				uniformOffsets.push_back(currentUniformOffset);
				currentUniformOffset = util::roundUpAlign(currentUniformOffset + sizeof(uniformBlock), ubos[sys.frameIndex].align);
			}
			verts[sys.frameIndex].flushAll();
			ubos[sys.frameIndex].flushAll();
		}

		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(buf, &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = { };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pass;
		renderPassInfo.framebuffer = fbos[sys.frameIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = sys.inst._extent;

		vkCmdBeginRenderPass(buf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		if (active) {
			for (int i = 0; i < groupings.size(); i++) {
				vkCmdBindVertexBuffers(buf, 0, 1, &verts[sys.frameIndex].buf, &vertOffsets[i]);

				vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &uboSets[sys.frameIndex], 1, &uniformOffsets[i]);

				vkCmdDraw(buf, groupings[i].data.size(), 1, 0, 0);
			}
		}

		vkCmdEndRenderPass(buf);

		vkEndCommandBuffer(buf);

		VkSubmitInfo subInfo = { };
		subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInfo.commandBufferCount = 1;
		subInfo.pCommandBuffers = &buf;
		vector<VkSemaphore> signalSems = getSignalSems();
		subInfo.signalSemaphoreCount = signalSems.size();
		subInfo.pSignalSemaphores = signalSems.data();
		vector<VkSemaphore> waitSems = getWaitSems();
		vector<VkPipelineStageFlags> waitFlags(waitSems.size(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		subInfo.waitSemaphoreCount = waitSems.size();
		subInfo.pWaitSemaphores = waitSems.data();
		subInfo.pWaitDstStageMask = waitFlags.data();

		vkQueueSubmit(sys.inst._graphicsQueue, 1, &subInfo, waitFences[sys.frameIndex]);
	}
	void immediatePass::renderPartial(uint32_t const& threadIndex) {
	}
	void immediatePass::postRender(uint32_t const& threadCount) {
	}
	immediatePass::immediatePass(system& sys, frameStore* fstore, fpath const& vert, fpath const& frag, bool transitionToRead) : sysNode(sys), frame(fstore) {
		this->transitionToRead = transitionToRead;

		this->vert = vert;
		this->frag = frag;

		for (int i = 0; i < SWAP_FRAMES; i++) {
			waitFences[i] = sys.inst.createFence(true);

			priBufs[i] = VK_NULL_HANDLE;

			ubos[i].init(&sys.inst, uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);

			verts[i].init(&sys.inst, vertSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
		}

		initializeDescriptors();
		initializeRenderPass();
		initializePipelineLayout();
		initializePipeline();
		initializeFramebuffers();

		{
			groupings.push_back({});
			grouping& gp = groupings.back();
			gp.color = vec3(1.0f, 1.0f, 1.0f);
			gp.data = {
				vec3(0.0f, 0.0f, 0.0f),
				vec3(1.0f, 0.0f, 0.0f),
				vec3(0.0f, 0.0f, 1.0f),
				vec3(0.0f, 0.0f, 0.0f),
				vec3(0.0f, 0.0f, 1.0f),
				vec3(1.0f, 0.0f, 0.0f)
			};
			gp.tr = glm::translate(vec3(0.0f, 0.0f, 0.0f));
		}
		{
			groupings.push_back({});
			grouping& gp = groupings.back();
			gp.color = vec3(1.0f, 1.0f, 1.0f);
			gp.data = {
				vec3(0.0f, 0.0f, 0.0f),
				vec3(1.0f, 0.0f, 0.0f),
				vec3(0.0f, 0.0f, 1.0f),
				vec3(0.0f, 0.0f, 0.0f),
				vec3(0.0f, 0.0f, 1.0f),
				vec3(1.0f, 0.0f, 0.0f)
			};
			gp.tr = glm::translate(vec3(1.0f, 0.0f, 0.0f));
		}
	}
	immediatePass::~immediatePass() {
	}
}