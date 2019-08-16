#pragma once

#include "citrus/graphics/system/instance.h"
#include "citrus/graphics/image.h"

namespace citrus::graphics {

	class buffer {
	public:
		instance*		inst;

		uint64_t		align;
		uint64_t		size;
		VkDeviceMemory	mem;
		VkBuffer		buf;
		uint8_t*		mapped;

		void flushAll();
		void flushRange(uint64_t start, uint64_t size);
		void init(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map);
		void free();

		buffer();
		buffer(instance* inst, uint64_t size, VkBufferUsageFlags usages, VkMemoryPropertyFlags props, bool map);
		~buffer();
	};

	class cubemapStore {
	public:
		instance*		inst;

		uint64_t const	size = 1024 * 1024 * 4 * 6 * 2; //number pixels * bytes per pixel * number sides * expected number of cubemaps
		uint64_t		align;
		
		buffer			staging;
		VkDeviceMemory	mem;

		struct cubemap {
			VkImage		img;
			VkImageView	view;
			VkSampler	samp;
			uint64_t	offset;
			uint64_t	nextAvailable;
		};
		vector<cubemap>	cubemaps;

		void addImage(image4b const& data);

		// allocates mem with size size and sets align
		void initMemory();
		
		cubemapStore(instance* inst);
		cubemapStore(instance* inst, vector<fpath> const& paths);
		~cubemapStore();
	};

	class frameStore {
	public:
		instance& inst;

		struct frameComponent {
			VkImage			img;
			VkImageView		view;
			VkSampler		samp;
			VkDeviceMemory	mem;
			inline void		destroy(instance const& inst) {
				vkDestroySampler(inst._device,samp, nullptr);
				
				vkDestroyImageView(inst._device, view, nullptr);
				
				vkDestroyImage(inst._device, img, nullptr);
				
				vkFreeMemory(inst._device, mem, nullptr);
			}
		};

		struct frame {
			frameComponent	color;
			frameComponent	index;
			frameComponent	depth;
		};

		frame				frames[SWAP_FRAMES];

		//buffer				tmpRes;
		//uint16_t			getPixelIndex(uint32_t frameIndex, uint32_t x, uint32_t y);

	private:
		void initColor();
		void initIndex();
		void initDepth();
	public:

		VkDescriptorImageInfo	getColorInfo(uint32_t const& index);
		VkDescriptorImageInfo	getIndexInfo(uint32_t const& index);
		VkDescriptorImageInfo	getDepthInfo(uint32_t const& index);
		vector<VkImageView>		getViews(uint32_t const& index);
		vector<VkAttachmentDescription> frameStore::getAttachmentDescriptions(bool transitionToRead);
		vector<VkAttachmentReference> getColorAttachmentRefs();
		VkAttachmentReference getDepthAttachmentRef();
		frameStore(instance& inst);
		~frameStore();
	};
}