#include "runtimeResource.h"

namespace citrus::graphics {
	uint16_t frameStore::getPixelIndex(uint32_t frameIndex, uint32_t x, uint32_t y) {
		VkCommandBuffer buf = inst.createCommandBuffer(inst._commandPool);

		VkCommandBufferBeginInfo inf = {};
		inf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(buf, &inf);

		{
			inst.pipelineBarrierLayoutChange(frames[frameIndex].index.img,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
			VkImageMemoryBarrier imgBarrier = {};
			imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imgBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imgBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imgBarrier.image = frames[frameIndex].index.img;
			imgBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imgBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
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
	}
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
		imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
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
	VkDescriptorImageInfo frameStore::getColorInfo(uint32_t const& index) {
		VkDescriptorImageInfo colorInfo = {};
		colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorInfo.imageView = frames[index].color.view;
		colorInfo.sampler = frames[index].color.samp;
		return colorInfo;
	}
	VkDescriptorImageInfo frameStore::getDepthInfo(uint32_t const& index) {
		VkDescriptorImageInfo depthInfo = {};
		depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthInfo.imageView = frames[index].depth.view;
		depthInfo.sampler = frames[index].depth.samp;
		return depthInfo;
	}
	VkDescriptorImageInfo frameStore::getIndexInfo(uint32_t const& index) {
		VkDescriptorImageInfo depthInfo = {};
		depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthInfo.imageView = frames[index].index.view;
		depthInfo.sampler = frames[index].index.samp;
		return depthInfo;
	}
	frameStore::frameStore(instance& inst) : inst(inst) {
		initColor();
		initDepth();
		initIndex();
		tmpRes.init(&inst, 2, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, true);
	}
	frameStore::~frameStore() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			vkDestroySampler(inst._device, frames[i].color.samp, nullptr);
			vkDestroySampler(inst._device, frames[i].depth.samp, nullptr);
			vkDestroySampler(inst._device, frames[i].index.samp, nullptr);

			vkDestroyImageView(inst._device, frames[i].color.view, nullptr);
			vkDestroyImageView(inst._device, frames[i].depth.view, nullptr);
			vkDestroyImageView(inst._device, frames[i].index.view, nullptr);

			vkDestroyImage(inst._device, frames[i].color.img, nullptr);
			vkDestroyImage(inst._device, frames[i].depth.img, nullptr);
			vkDestroyImage(inst._device, frames[i].index.img, nullptr);

			vkFreeMemory(inst._device, frames[i].color.mem, nullptr);
			vkFreeMemory(inst._device, frames[i].depth.mem, nullptr);
			vkFreeMemory(inst._device, frames[i].index.mem, nullptr);
		}
	}


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
	buffer::buffer() : inst(nullptr), size(0), mem(VK_NULL_HANDLE), buf(VK_NULL_HANDLE), mapped(nullptr) { }
	buffer::buffer(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map) : buffer() {
		init(inst, size, usages, props, map);
	}
	buffer::~buffer() {
		if (mapped) vkUnmapMemory(inst->_device, mem);
		if(buf != VK_NULL_HANDLE) vkDestroyBuffer(inst->_device, buf, nullptr);
		if (mem != VK_NULL_HANDLE) vkFreeMemory(inst->_device, mem, nullptr);
	}
}