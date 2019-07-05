#include "citrus/graphics/system/meshPass.h"

namespace citrus::graphics {
    void meshPass::initializeDescriptors() {s
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
	void meshPass::initializePipelineLayout() {
		VkPushConstantRange pcRange = { };
		pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pcRange.size = sizeof(vert_pcData);
		pcRange.offset = 0;

		VkPushConstantRange frag_pcRange = { };
		frag_pcRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_pcRange.size = sizeof(frag_pcData);
		frag_pcRange.offset = sizeof(vert_pcData);

		VkPushConstantRange ranges[2] = { pcRange, frag_pcRange };

		VkDescriptorSetLayout setLayouts[2] = { uboLayout, texLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 2;
		pipelineLayoutInfo.pPushConstantRanges = ranges;
		pipelineLayoutInfo.setLayoutCount = 2;
		pipelineLayoutInfo.pSetLayouts = setLayouts;

		if (vkCreatePipelineLayout(inst._device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) throw std::runtime_error("failed to create pipeline layout");
	}
	void meshPass::initializePipeline() {

		VkShaderModule vertModule, fragModule;
		VkPipelineShaderStageCreateInfo vertInfo, fragInfo;
		{
			string src = util::loadEntireFile(vert);

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
			string src = util::loadEntireFile(frag);

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
		pipelineInfo.pDepthStencilState = &depthStateInfo;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(inst._device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");

		vkDestroyShaderModule(inst._device, vertInfo.module, nullptr);
		vkDestroyShaderModule(inst._device, fragInfo.module, nullptr);
	}
    
	void meshPass::mapModels() {
		// check if attribs are supported
		uint32_t reqUse = (uint32_t)requiredUsages;
		for(int i = 0; i < sys.models.size(); i++) {
			uint32_t modelUse = (uint32_t)sys.models[i].desc.allUsage;
			if((modelUse & reqUse) != reqUse) continue;

			models.push_back(meshMappings.makePartialStructureView(sys.models[i].desc));
		}
	}
	
	meshPass::meshPass(system & sys, string const& vertLoc, string const& fragLoc) : passBase(sys) {
		vert = vertLoc;
		frag = fragLoc;

		map<meshAttributeUsage, uint32_t> locMap;
		locMap[meshAttributeUsage::positionType] = 0;
		locMap[meshAttributeUsage::normalType] = 1;
		locMap[meshAttributeUsage::uvType] = 2;
		meshMappings = meshUsageLocationMapping(locMap);

		initializeDescriptors();
		initializeRenderPass();
		initializePipelineLayout();
		initializePipeline();
		initializeFramebuffers();
		mapModels();
	}
    meshPass::~meshPass() {
        
    }
}