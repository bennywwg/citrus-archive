#pragma once

#include "citrus/graphics/system/renderSystem.h"

namespace citrus::graphics {
	class frameStore {
	public:
		instance&			inst;

		struct frameComponent {
			VkImage			img;
			VkImageView		view;
			VkSampler		samp;
			VkDeviceMemory	mem;
		};

		struct frame {
			frameComponent	color;
			frameComponent	depth;
			frameComponent	index;
		};

		frame				frames[SWAP_FRAMES];

	private:
		void initColor();
		void initDepth();
		void initIndex();
	public:

		VkDescriptorImageInfo frameStore::getColorInfo(uint32_t const& index);
		VkDescriptorImageInfo frameStore::getDepthInfo(uint32_t const& index);
		VkDescriptorImageInfo frameStore::getIndexInfo(uint32_t const& index);

		frameStore(instance& inst);
		~frameStore();
	};

	class buffer {
	public:
		instance*		inst;

		uint64_t		align;
		uint64_t		size;
		VkDeviceMemory	mem;
		VkBuffer		buf;
		uint8_t*		mapped;

		void flushRange(uint64_t start, uint64_t size);
		void init(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map);

		buffer();
		buffer(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map);
		~buffer();
	};
}