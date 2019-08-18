#include "runtimeResource.h"

namespace citrus::graphics {
	/*uint16_t frameStore::getPixelIndex(uint32_t frameIndex, uint32_t x, uint32_t y) {
		VkCommandBuffer buf = inst.createCommandBuffer(inst._commandPool);

		VkCommandBufferBeginInfo inf = {};
		inf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(buf, &inf);

		{
			//inst.pipelineBarrierLayoutChange(frames[frameIndex].index.img,
			//	VK_IMAGE_ASPECT_COLOR_BIT,
			//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			//	VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT,
			//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
			VkImageMemoryBarrier imgBarrier = {};
			imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imgBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imgBarrier.image = frames[frameIndex].index.img;
			imgBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imgBarrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(buf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgBarrier);
		}

		VkBufferImageCopy region = {};
		region.imageOffset = { (int32_t) x, (int32_t)y, 0 };
		region.imageExtent = { 1, 1, 1 };
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.mipLevel = 0;
		region.bufferOffset = 0;
		vkCmdCopyImageToBuffer(buf, frames[frameIndex].index.img, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, tmpRes.buf, 1, &region);

		{
			VkBufferMemoryBarrier bufBarrier = {};
			bufBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			bufBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
			bufBarrier.offset = 0;
			bufBarrier.size = 2;
			bufBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufBarrier.buffer = tmpRes.buf;
			vkCmdPipelineBarrier(buf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_HOST_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT, 0, nullptr, 1, &bufBarrier, 0, nullptr);
		}

		vkEndCommandBuffer(buf);

		VkFence fen = inst.createFence(false);

		VkSubmitInfo subInf = {};
		subInf.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInf.commandBufferCount = 1;
		subInf.pCommandBuffers = &buf;
		vkQueueSubmit(inst._graphicsQueue, 1, &subInf, fen);

		inst.waitForFence(fen);

		inst.destroyCommandBuffer(buf, inst._commandPool);

		inst.destroyFence(fen);

		VkMappedMemoryRange invRange = {};
		invRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		invRange.memory = tmpRes.mem;
		invRange.offset = 0;
		invRange.size = 2;
		vkInvalidateMappedMemoryRanges(inst._device, 1, &invRange);

		uint16_t finalRes = *(uint16_t*)(tmpRes.mapped);
		
		return finalRes;
	}*/
	void frameStore::initColor() {
		VkImageCreateInfo imgInfo = {};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.extent.width = inst.width;
		imgInfo.extent.height = inst.height;
		imgInfo.extent.depth = 1;
		imgInfo.mipLevels = 1;
		imgInfo.arrayLayers = 1;
		imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

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

		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateImage(inst._device, &imgInfo, nullptr, &frames[i].color.img) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements reqs;
			vkGetImageMemoryRequirements(inst._device, frames[i].color.img, &reqs);
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.memoryTypeIndex = inst.findMemoryType(reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			allocInfo.allocationSize = reqs.size;

			vkAllocateMemory(inst._device, &allocInfo, nullptr, &frames[i].color.mem);

			vkBindImageMemory(inst._device, frames[i].color.img, frames[i].color.mem, 0);

			viewInfo.image = frames[i].color.img;
			vkCreateImageView(inst._device, &viewInfo, nullptr, &frames[i].color.view);

			vkCreateSampler(inst._device, &sampInfo, nullptr, &frames[i].color.samp);
		}
	}
	void frameStore::initIndex() {
		VkImageCreateInfo imgInfo = {};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.extent.width = inst.width;
		imgInfo.extent.height = inst.height;
		imgInfo.extent.depth = 1;
		imgInfo.mipLevels = 1;
		imgInfo.arrayLayers = 1;
		imgInfo.format = VK_FORMAT_R16_UINT;
		imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;// | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R16_UINT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

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

		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateImage(inst._device, &imgInfo, nullptr, &frames[i].index.img) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements reqs;
			vkGetImageMemoryRequirements(inst._device, frames[i].index.img, &reqs);
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.memoryTypeIndex = inst.findMemoryType(reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			allocInfo.allocationSize = reqs.size;

			vkAllocateMemory(inst._device, &allocInfo, nullptr, &frames[i].index.mem);

			vkBindImageMemory(inst._device, frames[i].index.img, frames[i].index.mem, 0);

			viewInfo.image = frames[i].index.img;
			vkCreateImageView(inst._device, &viewInfo, nullptr, &frames[i].index.view);

			vkCreateSampler(inst._device, &sampInfo, nullptr, &frames[i].index.samp);
		}
	}
	void frameStore::initDepth() {
		VkImageCreateInfo imgInfo = {};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.extent.width = inst.width;
		imgInfo.extent.height = inst.height;
		imgInfo.extent.depth = 1;
		imgInfo.mipLevels = 1;
		imgInfo.arrayLayers = 1;
		imgInfo.format = inst.findDepthFormat();
		imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = inst.findDepthFormat();
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

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

		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateImage(inst._device, &imgInfo, nullptr, &frames[i].depth.img) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements reqs;
			vkGetImageMemoryRequirements(inst._device, frames[i].depth.img, &reqs);
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.memoryTypeIndex = inst.findMemoryType(reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			allocInfo.allocationSize = reqs.size;

			vkAllocateMemory(inst._device, &allocInfo, nullptr, &frames[i].depth.mem);

			vkBindImageMemory(inst._device, frames[i].depth.img, frames[i].depth.mem, 0);

			viewInfo.image = frames[i].depth.img;
			vkCreateImageView(inst._device, &viewInfo, nullptr, &frames[i].depth.view);

			vkCreateSampler(inst._device, &sampInfo, nullptr, &frames[i].depth.samp);
		}
	}

	VkDescriptorImageInfo frameStore::getColorInfo(uint32_t const& index) {
		VkDescriptorImageInfo colorInfo = {};
		colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorInfo.imageView = frames[index].color.view;
		colorInfo.sampler = frames[index].color.samp;
		return colorInfo;
	}
	VkDescriptorImageInfo frameStore::getIndexInfo(uint32_t const& index) {
		VkDescriptorImageInfo indexInfo = {};
		indexInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		indexInfo.imageView = frames[index].index.view;
		indexInfo.sampler = frames[index].index.samp;
		return indexInfo;
	}
	VkDescriptorImageInfo frameStore::getDepthInfo(uint32_t const& index) {
		VkDescriptorImageInfo depthInfo = {};
		depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthInfo.imageView = frames[index].depth.view;
		depthInfo.sampler = frames[index].depth.samp;
		return depthInfo;
	}
	vector<VkImageView> frameStore::getViews(uint32_t const& index) {
		return { frames[index].color.view, frames[index].index.view, frames[index].depth.view };
	}
	vector<VkAttachmentDescription> frameStore::getAttachmentDescriptions(bool transitionToRead) {
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
		depthAttachment.format = inst.findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = transitionToRead ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		return { colorAttachment, indexAttachment, depthAttachment };
	}
	vector<VkAttachmentReference> frameStore::getColorAttachmentRefs() {
		VkAttachmentReference colorAttachmentRef = { };
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference indexAttachmentRef = { };
		indexAttachmentRef.attachment = 1;
		indexAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		return { colorAttachmentRef, indexAttachmentRef };
	}
	VkAttachmentReference frameStore::getDepthAttachmentRef() {
		VkAttachmentReference depthAttachmentRef = { };
		depthAttachmentRef.attachment = 2;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		return depthAttachmentRef;
	}
	frameStore::frameStore(instance& inst) : inst(inst) {
		initColor();
		initIndex();
		initDepth();
		//tmpRes.init(&inst, 2, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
	}
	frameStore::~frameStore() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			frames[i].color.destroy(inst);
			frames[i].index.destroy(inst);
			frames[i].depth.destroy(inst);
		}
	}

	/*void memory::flushAll() {
		VkMappedMemoryRange range = { };
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.memory = mem;
		range.offset = 0;
		range.pNext = nullptr;
		range.size = VK_WHOLE_SIZE;
		if (vkFlushMappedMemoryRanges(inst->_device, 1, &range) != VK_SUCCESS) throw std::runtime_error("failed to map flush memory range");
	}
	void memory::flushRange(uint64_t start, uint64_t size) {
		VkMappedMemoryRange range = { };
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.memory = mem;
		range.offset = util::roundDownAlign(start, align);
		range.pNext = nullptr;
		range.size = util::roundUpAlign(size, align);
		if (vkFlushMappedMemoryRanges(inst->_device, 1, &range) != VK_SUCCESS) throw std::runtime_error("failed to map flush memory range");
	}
	void memory::init(instance* inst, uint64_t size, VkMemoryPropertyFlags props, bool map) {
		this->size = size;

		this->inst = inst;

		flushAlign = inst->minFlushRangeAlignment();

		VkMemoryAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = size;
		allocInfo.memoryTypeIndex = inst->findMemoryType(memRequirements.memoryTypeBits, props);

		if (vkAllocateMemory(inst->_device, &allocInfo, nullptr, &mem) != VK_SUCCESS) throw std::runtime_error("failed to allocate buffer memory!");

		if (vkBindBufferMemory(inst->_device, buf, mem, 0) != VK_SUCCESS) throw std::runtime_error("failed to bind buffer memory");

		if (map) {
			vkMapMemory(inst->_device, mem, 0, size, 0, (void**)& mapped);
		}
	}
	void memory::free() {
		if (mapped) vkUnmapMemory(inst->_device, mem);
		if (mem != VK_NULL_HANDLE) vkFreeMemory(inst->_device, mem, nullptr);
		mapped = nullptr;
		size = 0;
		mem = VK_NULL_HANDLE;
		inst = nullptr;
	}
	memory::memory() : inst(nullptr), size(0), mem(VK_NULL_HANDLE), mapped(nullptr) { }
	memory::memory(instance* inst, uint64_t size, VkMemoryPropertyFlags props, bool map) : memory() {
		init(inst, size,  props, map);
	}
	memory::~memory() {
		free();
	}*/

	void buffer::flushAll() {
		VkMappedMemoryRange range = { };
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.memory = mem;
		range.offset = 0;
		range.pNext = nullptr;
		range.size = VK_WHOLE_SIZE;
		if (vkFlushMappedMemoryRanges(inst->_device, 1, &range) != VK_SUCCESS) throw std::runtime_error("failed to map flush memory range");
	}
	void buffer::flushRange(uint64_t start, uint64_t size) {
		VkMappedMemoryRange range = { };
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.memory = mem;
		range.offset = util::roundDownAlign(start, align);
		range.pNext = nullptr;
		range.size = util::roundUpAlign(size, align);
		if (vkFlushMappedMemoryRanges(inst->_device, 1, &range) != VK_SUCCESS) throw std::runtime_error("failed to map flush memory range");
	}
	void buffer::init(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map) {
		this->size = size;

		this->inst = inst;
		VkBufferCreateInfo bufferInfo = { };
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usages;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(inst->_device, &bufferInfo, nullptr, &buf) != VK_SUCCESS) throw std::runtime_error("failed to create memory buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(inst->_device, buf, &memRequirements);

		align = memRequirements.alignment;

		VkMemoryAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = inst->findMemoryType(memRequirements.memoryTypeBits, props);

		if (vkAllocateMemory(inst->_device, &allocInfo, nullptr, &mem) != VK_SUCCESS) throw std::runtime_error("failed to allocate buffer memory!");

		if (vkBindBufferMemory(inst->_device, buf, mem, 0) != VK_SUCCESS) throw std::runtime_error("failed to bind buffer memory");

		if (map) {
			vkMapMemory(inst->_device, mem, 0, size, 0, (void**)& mapped);
		}
	}
	void buffer::free() {
		if (mapped) vkUnmapMemory(inst->_device, mem);
		if (buf != VK_NULL_HANDLE) vkDestroyBuffer(inst->_device, buf, nullptr);
		if (mem != VK_NULL_HANDLE) vkFreeMemory(inst->_device, mem, nullptr);
		mapped = nullptr;
		size = 0;
		mem = VK_NULL_HANDLE;
		buf = VK_NULL_HANDLE;
		inst = nullptr;
		align = 0;
	}
	buffer::buffer() : inst(nullptr), size(0), mem(VK_NULL_HANDLE), buf(VK_NULL_HANDLE), mapped(nullptr) { }
	buffer::buffer(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map) : buffer() {
		init(inst, size, usages, props, map);
	}
	buffer::~buffer() {
		free();
	}


	cubemapStore::~cubemapStore() {
		for (cubemap const& cm : cubemaps) {
			vkDestroyImageView(inst->_device, cm.view, nullptr);
			vkDestroyImage(inst->_device, cm.img, nullptr);
			vkDestroySampler(inst->_device, cm.samp, nullptr);
		}

		vkFreeMemory(inst->_device, mem, nullptr);
	}
	void cubemapStore::addImage(image4b const& data) {
		uint64_t nextAvailable = cubemaps.empty() ? 0 : cubemaps.back().nextAvailable;

		cubemaps.push_back({});
		cubemap& res = cubemaps.back();

		VkImageCreateInfo imgInfo = {};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgInfo.arrayLayers = 6;
		imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imgInfo.extent = { data.width(), data.height() / 6, 1 };
		imgInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		imgInfo.mipLevels = 1;

		vkCreateImage(inst->_device, &imgInfo, nullptr, &res.img);

		VkMemoryRequirements reqs = {};
		vkGetImageMemoryRequirements(inst->_device, res.img, &reqs);

		res.offset = util::roundUpAlign(nextAvailable, align);
		res.nextAvailable = res.offset + reqs.size;
		
		vkBindImageMemory(inst->_device, res.img, mem, res.offset);

		VkSamplerCreateInfo sampInfo = {};
		sampInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampInfo.minFilter = VK_FILTER_LINEAR;
		sampInfo.magFilter = VK_FILTER_LINEAR;
		sampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampInfo.mipLodBias = 0.0f;
		sampInfo.compareOp = VK_COMPARE_OP_NEVER;
		sampInfo.compareEnable = false;
		sampInfo.minLod = 0.0f;
		sampInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampInfo.maxAnisotropy = 1.0f;

		vkCreateSampler(inst->_device, &sampInfo, nullptr, &res.samp);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		viewInfo.subresourceRange.layerCount = 6;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.image = res.img;

		vkCreateImageView(inst->_device, &viewInfo, nullptr, &res.view);

		memcpy(staging.mapped, data.data(), data.size());
		staging.flushAll();

		VkCommandBuffer commandBuffer = inst->createCommandBuffer(inst->_commandPool);

		VkBufferImageCopy regions[6];
		for (int layer = 0; layer < 6; layer++) {
			VkBufferImageCopy& region = regions[layer];
			region = {};

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = layer;
			region.imageSubresource.layerCount = 1;
			region.imageExtent.width = data.width();
			region.imageExtent.height = data.height() / 6;
			region.imageExtent.depth = 1;
			region.bufferOffset = res.offset + (layer * data.width() * data.height() / 6 * 4);
		}

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkImageMemoryBarrier imgBarrier = { };
		imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imgBarrier.image = res.img;
		imgBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imgBarrier.srcAccessMask = 0;
		imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imgBarrier.subresourceRange.baseMipLevel = 0;
		imgBarrier.subresourceRange.levelCount = 1;
		imgBarrier.subresourceRange.baseArrayLayer = 0;
		imgBarrier.subresourceRange.layerCount = 6;

		VkImageMemoryBarrier secBarrier = { };
		secBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		secBarrier.image = res.img;
		secBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		secBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		secBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		secBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		secBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		secBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		secBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		secBarrier.subresourceRange.baseMipLevel = 0;
		secBarrier.subresourceRange.levelCount = 1;
		secBarrier.subresourceRange.baseArrayLayer = 0;
		secBarrier.subresourceRange.layerCount = 6;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT,
			0, nullptr,
			0, nullptr,
			1, &imgBarrier);

		vkCmdCopyBufferToImage(
			commandBuffer,
			staging.buf,
			res.img,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			6,
			regions
		);

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_DEPENDENCY_DEVICE_GROUP_BIT,
			0, nullptr,
			0, nullptr,
			1, &secBarrier);

		vkEndCommandBuffer(commandBuffer);

		inst->submitWaitDestroy(inst->_graphicsQueue, commandBuffer, inst->_commandPool);
	}

	void cubemapStore::initMemory() {
		uint32_t requiredTypeBits;
		{ // dummy cubemap for memory type bits
			VkImageCreateInfo imgInfo = {};
			imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imgInfo.imageType = VK_IMAGE_TYPE_2D;
			imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imgInfo.arrayLayers = 6;
			imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imgInfo.extent = { 512, 512, 1 };
			imgInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			imgInfo.mipLevels = 1;

			VkImage tmpImg = VK_NULL_HANDLE;
			vkCreateImage(inst->_device, &imgInfo, nullptr, &tmpImg);

			VkMemoryRequirements reqs = {};
			vkGetImageMemoryRequirements(inst->_device, tmpImg, &reqs);

			vkDestroyImage(inst->_device, tmpImg, nullptr);

			align = reqs.alignment;
			requiredTypeBits = reqs.memoryTypeBits;
		}

		VkMemoryAllocateInfo memInfo = {};
		memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memInfo.allocationSize = size;
		memInfo.memoryTypeIndex = inst->findMemoryType(requiredTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(inst->_device, &memInfo, nullptr, &mem);

		staging.init(inst, 1024 * 1024 * 4 * 6 * 2, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
	}
	cubemapStore::cubemapStore(instance* inst) : inst(inst) {
		initMemory();
	}
	cubemapStore::cubemapStore(instance* inst, vector<fpath> const& paths) : cubemapStore(inst) {
		for (fpath const& path : paths) {
			addImage(image4b(path.string()));
		}
	}
}