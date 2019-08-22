#include "clearFrame.h"

namespace citrus::graphics {
	void clearFrame::initializeRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription indexAttachment = {};
		indexAttachment.format = VK_FORMAT_R16_UINT;
		indexAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		indexAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		indexAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		indexAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		indexAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		indexAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		indexAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = sys.inst.findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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

	void clearFrame::initializeFramebuffers() {
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

	void clearFrame::preRender(uint32_t const& threadCount) {
		sys.inst.waitForFence(waitFences[sys.frameIndex]);
		sys.inst.resetFence(waitFences[sys.frameIndex]);

		if (frameNum > SWAP_FRAMES) {
			selectedIndex = frame->getPixelIndex(sys.frameIndex, cursorX, cursorY);
		}
		frameNum++;

		VkCommandBuffer& buf = priBufs[sys.frameIndex];
		if (buf != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(buf, sys.inst._commandPool);
		buf = sys.inst.createCommandBuffer(sys.inst._commandPool);

		VkCommandBufferBeginInfo begInf = { };
		begInf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begInf.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(buf, &begInf);

		VkClearValue clearValues[4] = { };
		clearValues[0].color = { 0.0f, 0.0f, 0.0f };
		clearValues[1].color.uint32[0] = 0;
		clearValues[2].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassInfo = { };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pass;
		renderPassInfo.framebuffer = fbos[sys.frameIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = sys.inst._extent;
		renderPassInfo.clearValueCount = 4;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(buf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdEndRenderPass(buf);

		vkEndCommandBuffer(buf);

		VkSubmitInfo subInfo = { };
		subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInfo.commandBufferCount = 1;
		subInfo.pCommandBuffers = &buf;
		vector<VkSemaphore> signalSems = getSignalSems();
		subInfo.signalSemaphoreCount = signalSems.size();
		subInfo.pSignalSemaphores = signalSems.data();

		vkQueueSubmit(sys.inst._graphicsQueue, 1, &subInfo, waitFences[sys.frameIndex]);
	}

	void clearFrame::renderPartial(uint32_t const& threadIndex) {
	}

	void clearFrame::postRender(uint32_t const& threadCount) {
	}

	clearFrame::clearFrame(system& sys, frameStore* fstore) : sysNode(sys), frame(fstore) {
		initializeRenderPass();
		initializeFramebuffers();
		
		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			waitFences[i] = sys.inst.createFence(true);
			priBufs[i] = VK_NULL_HANDLE;
		}
	}

	clearFrame::~clearFrame() {
		vkDestroyRenderPass(sys.inst._device, pass, nullptr);
		for (uint32_t i = 0; i < SWAP_FRAMES; i++) {
			vkDestroyFramebuffer(sys.inst._device, fbos[i], nullptr);
			sys.inst.destroyFence(waitFences[i]);
			if(priBufs[i] != VK_NULL_HANDLE) sys.inst.destroyCommandBuffer(priBufs[i], sys.inst._commandPool);
		}
	}
	//void layoutTransition::preRender(uint32_t const& threadCount) {
	//	VkCommandBuffer& buf = priBufs[sys.frameIndex];

	//	VkImageMemoryBarrier barrier[3] = {};
	//	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//	barrier.oldLayout = oldLayout;
	//	barrier.newLayout = newLayout;
	//	barrier.srcAccessMask = srcAccess;
	//	barrier.dstAccessMask = dstAccess;
	//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.image = frame->frames[sys.frameIndex].color.img;
	//	barrier.subresourceRange.aspectMask = aspect;
	//	barrier.subresourceRange.baseMipLevel = 0;
	//	barrier.subresourceRange.levelCount = 1;
	//	barrier.subresourceRange.baseArrayLayer = 0;
	//	barrier.subresourceRange.layerCount = 1;

	//	VkCommandBufferBeginInfo beginInfo = {};
	//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//	vkBeginCommandBuffer(buf, &beginInfo);

	//	vkCmdPipelineBarrier(
	//		buf,
	//		srcStage, dstStage,
	//		0,
	//		0, nullptr,
	//		0, nullptr,
	//		3, barriers
	//	);

	//	vkEndCommandBuffer(commandBuffer);
	//}
	//void layoutTransition::renderPartial(uint32_t const& threadIndex) {
	//}
	//void layoutTransition::postRender(uint32_t const& threadCount) {
	//}
	//layoutTransition::layoutTransition(system& sys, frameStore* fstore) : sysNode(sys), frame(fstore) {
	//}
	//layoutTransition::~layoutTransition() {
	//}
}