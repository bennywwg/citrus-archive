#include <graphics/vulkan/vkBuffer.h>
#include <graphics/vulkan/instance.h>

namespace citrus::graphics {
	vkBuffer::vkBuffer(instance& inst) {
		VkBufferCreateInfo bufferInfo = { };
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = 1;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		VkResult res = vkCreateBuffer(inst._device, &bufferInfo, nullptr, &_buffer);
		if(res != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer");
		}

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}