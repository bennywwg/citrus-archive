#pragma once

#include <graphics/vulkan/instance.h>

namespace citrus::graphics {
	class vkBuffer {
		instance& _inst;
		VkBuffer _buffer;
		VkDeviceMemory _memory;

		public:
		void waitUntilReady();

		void bufferData(uint64_t start, const void* data, uint64_t size, fenceProc* proc = nullptr);
		void bufferData(uint64_t start, std::function<void(const void*)> fillFunc, uint64_t size, fenceProc* proc = nullptr);

		vkBuffer(instance& inst, uint64_t size);
		~vkBuffer();
	};
}