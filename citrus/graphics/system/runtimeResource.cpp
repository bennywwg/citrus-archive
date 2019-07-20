#include "runtimeResource.h"

namespace citrus::graphics {
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
		imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
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
}