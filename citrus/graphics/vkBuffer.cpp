#include <graphics/vulkan/vkBuffer.h>
#include <functional>

namespace citrus::graphics {
	void vkBuffer::waitUntilReady() {
	}

	void vkBuffer::bufferData(uint64_t start, const void* data, uint64_t size, fenceProc* proc) {
		_inst.fillBuffer(_buffer, size, start, data, proc);
	}
	void vkBuffer::bufferData(uint64_t start, std::function<void(const void*)> fillFunc, uint64_t size, fenceProc* proc) {
		_inst.fillBuffer(_buffer, size, start, fillFunc, proc);
	}

	vkBuffer::vkBuffer(instance& inst, uint64_t size) : _inst(inst) {
		_inst.createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory);
	}
	vkBuffer::~vkBuffer() {
		vkFreeMemory(_inst._device, _memory, nullptr);
		vkDestroyBuffer(_inst._device, _buffer, nullptr);
	}
}