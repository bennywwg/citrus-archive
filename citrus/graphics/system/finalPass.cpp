#include "citrus/graphics/system/finalPass.h"
#include <exception>
#include <stdexcept>
#include <iostream>
#include <array>

namespace citrus::graphics {
	void finalPass::fillCommandBuffer(uint32_t frameIndex, VkDescriptorImageInfo colorInfo, VkDescriptorImageInfo depthInfo, VkDescriptorImageInfo indexInfo) {
		VkWriteDescriptorSet writes[3] = { };
		writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[0].dstSet = _set;
		writes[0].dstBinding = 0;
		writes[0].dstArrayElement = 0;
		writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[0].descriptorCount = 1;
		writes[0].pImageInfo = &colorInfo;
		writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[1].dstSet = _set;
		writes[1].dstBinding = 1;
		writes[1].dstArrayElement = 0;
		writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[1].descriptorCount = 1;
		writes[1].pImageInfo = &depthInfo;
		writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[2].dstSet = _set;
		writes[2].dstBinding = 2;
		writes[2].dstArrayElement = 0;
		writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[2].descriptorCount = 1;
		writes[2].pImageInfo = &indexInfo;
		vkUpdateDescriptorSets(sys.inst._device, 2, writes, 0, nullptr);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(cbufs[frameIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.framebuffer = targets[frameIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { sys.inst.width, sys.inst.height};

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(cbufs[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cbufs[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

		vkCmdBindDescriptorSets(cbufs[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_set, 0, nullptr);

		vkCmdDraw(cbufs[frameIndex], 6, 1, 0, 0);

		vkCmdEndRenderPass(cbufs[frameIndex]);
		if (vkEndCommandBuffer(cbufs[frameIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void finalPass::submit(uint32_t index, vector<VkSemaphore> waits) {
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cbufs[index];

		vector<VkPipelineStageFlags> waitStages(waits.size(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		submitInfo.waitSemaphoreCount = waits.size();
		submitInfo.pWaitSemaphores = waits.data();
		submitInfo.pWaitDstStageMask = waitStages.data();

		submitInfo.signalSemaphoreCount = 1;
		VkSemaphore signalSem = getSignalSem();
		submitInfo.pSignalSemaphores = &signalSem;

		vkQueueSubmit(sys.inst._graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	}

	void finalPass::initDescriptorsAndLayouts() {
		VkDescriptorSetLayoutBinding texBindings[2] = { };
		texBindings[0].binding = 0;
		texBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texBindings[0].descriptorCount = 1;
		texBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		texBindings[1].binding = 1;
		texBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texBindings[1].descriptorCount = 1;
		texBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = { };
		descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutInfo.bindingCount = 2;
		descriptorLayoutInfo.pBindings = texBindings;

		if (vkCreateDescriptorSetLayout(sys.inst._device, &descriptorLayoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		VkDescriptorPoolSize poolSize = { };
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 2;

		VkDescriptorPoolCreateInfo poolInfo = { };
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(sys.inst._device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

		VkDescriptorSetAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &_descriptorSetLayout;

		if (vkAllocateDescriptorSets(sys.inst._device, &allocInfo, &_set) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor set!");
		}
	}

	void finalPass::preRender(uint32_t const& threadCount) {
		frameIndex = win.getNextFrameIndex(frameReadySem);
		fillCommandBuffer(frameIndex, prevPass.frame->getColorInfo(frameIndex), prevPass.frame->getDepthInfo(frameIndex), prevPass.frame->getIndexInfo(frameIndex));
		vector<VkSemaphore> waits = getWaitSems();
		waits.push_back(frameReadySem);
		
		submit(frameIndex, waits);

		win.present(frameIndex, getSignalSem());
	}

	void finalPass::renderPartial(uint32_t const& threadIndex) {
		//nothing to do
	}

	void finalPass::postRender(uint32_t const& threadCount) {
		//nothing to do
	}

	finalPass::finalPass(system& sys, window& win, meshPass& prev, fpath vertLoc, fpath fragLoc) : sysNode(sys), frameReadySem(sys.inst.createSemaphore()), win(win), prevPass(prev) {
		frameIndex = 0;

		VkShaderModule vertModule = VK_NULL_HANDLE;
		VkShaderModule fragModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo vertCreateInfo = { };
		VkPipelineShaderStageCreateInfo fragCreateInfo = { };
		try {
			{
				string src = util::loadEntireFile(vertLoc.string());

				VkShaderModuleCreateInfo createInfo = { };
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = src.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

				if (vkCreateShaderModule(sys.inst._device, &createInfo, nullptr, &vertModule) != VK_SUCCESS) {
					vertModule = VK_NULL_HANDLE;
					throw std::runtime_error("Failed to create vert shader module");
				}

				vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertCreateInfo.module = vertModule;
				vertCreateInfo.pName = "main";
			}
			{
				string src = util::loadEntireFile(fragLoc.string());

				VkShaderModuleCreateInfo createInfo = { };
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = src.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

				if (vkCreateShaderModule(sys.inst._device, &createInfo, nullptr, &fragModule) != VK_SUCCESS) {
					fragModule = VK_NULL_HANDLE;
					throw std::runtime_error("Failed to create frag shader module");
				}

				fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragCreateInfo.module = fragModule;
				fragCreateInfo.pName = "main";
			}
		}
		catch (std::runtime_error & rt) {
			std::cout << (rt.what() + std::string("\n"));
			return;
		}

		initDescriptorsAndLayouts();

		vector<VkPipelineShaderStageCreateInfo> stages = vector<VkPipelineShaderStageCreateInfo>{ vertCreateInfo, fragCreateInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

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

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(sys.inst._device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkAttachmentDescription colorAttachment = { };
		colorAttachment.format = sys.inst._surfaceFormat.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = { };
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

		if (vkCreateRenderPass(sys.inst._device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
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

		if (vkCreateGraphicsPipelines(sys.inst._device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		if (vertModule != VK_NULL_HANDLE) vkDestroyShaderModule(sys.inst._device, vertModule, nullptr);
		if (fragModule != VK_NULL_HANDLE) vkDestroyShaderModule(sys.inst._device, fragModule, nullptr);



		for (int i = 0; i < sys.inst._swapChainImageViews.size(); i++) {
			VkImageView view = sys.inst._swapChainImageViews[i];
			VkImageView attachments[] = {
				view
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = sys.inst.width;
			framebufferInfo.height = sys.inst.height;
			framebufferInfo.layers = 1;

			VkFramebuffer fbo;
			if (vkCreateFramebuffer(sys.inst._device, &framebufferInfo, nullptr, &fbo) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}

			VkCommandBuffer buf;
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = sys.inst._commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			if (vkAllocateCommandBuffers(sys.inst._device, &allocInfo, &buf) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}

			targets.push_back(fbo);
			cbufs.push_back(buf);
		}
	}
	finalPass::~finalPass() {
		if (_descriptorSetLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(sys.inst._device, _descriptorSetLayout, nullptr);
		if (_descriptorPool != VK_NULL_HANDLE) vkDestroyDescriptorPool(sys.inst._device, _descriptorPool, nullptr);
		for (int i = 0; i < targets.size(); i++) {
			vkDestroyFramebuffer(sys.inst._device, targets[i], nullptr);
			vkFreeCommandBuffers(sys.inst._device, sys.inst._commandPool, 1, &cbufs[i]);
		}
		if (_pipeline != VK_NULL_HANDLE) vkDestroyPipeline(sys.inst._device, _pipeline, nullptr);
		if (_renderPass != VK_NULL_HANDLE) vkDestroyRenderPass(sys.inst._device, _renderPass, nullptr);
		if (_pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(sys.inst._device, _pipelineLayout, nullptr);
	}
}