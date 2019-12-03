#pragma once

#define GLFW_INCLUDE_VULKAN

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3.h>

#include <functional>
#include <vector>

#define SWAP_FRAMES 2

namespace citrus {

	class instance;

	struct ctDynamicOffsetBuffer {
		VkBuffer		buf;
		VkDeviceMemory	mem;
		uint64_t		size;
		uint64_t		align;
		void* mapped;
	};


	class instance {
	public:

		VkInstance _instance;

		uint32_t width, height;

		std::vector<VkExtensionProperties> _availableExtensions;
		std::vector<VkLayerProperties> _availableLayers;
		VkDebugUtilsMessengerEXT _callback;
		std::vector<VkPhysicalDevice> _availableDevices;
		std::vector<VkPhysicalDevice> _suitableDevices;
		VkPhysicalDevice _chosenDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;
		VkSurfaceKHR _surface;
		VkSurfaceFormatKHR _surfaceFormat;
		VkPresentModeKHR _presentMode;
		VkExtent2D _extent;
		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;
		VkCommandPool _commandPool;
		VkSemaphore _imgAvailableSemaphore, _renderFinishedSemaphore;

		uint64_t	flushAlign;

		void initInstance(std::string name);
		void destroyInstance();

		std::vector<const char*> getRequiredExtensions();
		void loadExtensions();
		void checkExtensions();

		std::vector<const char*> getRequiredLayers();
		void loadValidationLayers();
		void checkValidationLayers();
		void initDebugCallback();
		void destroyDebugCallback();

		std::vector<const char*> getRequiredDeviceExtensions();
		bool deviceHasRequiredExtensions(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		void loadPhysicalDevices();
		void choosePhysicalDevice();
		void initDevice();
		void destroyDevice();

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();

		void initSurface(GLFWwindow* win);
		void destroySurface();
		void chooseSurfaceFormat();
		void chooseSurfacePresentMode();
		void chooseSurfaceExtent(uint32_t width, uint32_t height);
		void initSwapChain();
		void destroySwapChain();
		void initFrameBuffers();

		void initCommandPool();
		void destroyCommandPool();

		void initSemaphores();
		void destroySemaphores();

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void initMemory();

		struct allocator {
			struct allocBlock {
				uint64_t addr;
				uint64_t size;
			};

			std::string name;
			instance& inst;
			VkDeviceMemory mem;
			VkBuffer buf;
			uint64_t size;
			uint64_t alignment;
			std::vector<allocBlock> blocks;

			uint64_t alloc(uint64_t size);
			void free(uint64_t addr);
			void initBuffer(uint64_t size, uint64_t align, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
			void initImage(uint64_t size, uint64_t align, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
			void freeResources();

			allocator(instance& inst);
		};

		allocator vertexMem;
		allocator indexMem;
		allocator uniformMem;
		allocator textureMem;
		allocator fboMem;
		allocator stagingMem;

		void mapUnmapMemory(VkDeviceMemory dstMemory, uint64_t size, uint64_t start, void* data);
		void fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, void* data);
		void fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, std::function<void(void*)> fillFunc);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size, uint64_t srcStart, uint64_t dstStart);
		void copyBufferToImage(VkBuffer stagingBuf, uint64_t start, VkImage image, uint32_t width, uint32_t height);
		void pipelineBarrierLayoutChange(VkImage image,
			VkImageAspectFlags aspect,
			VkImageLayout oldLayout, VkImageLayout newLayout,
			VkAccessFlags srcAccess, VkAccessFlags dstAccess,
			VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);

		void submitWaitDestroy(VkQueue queue, VkCommandBuffer buf, VkCommandPool pool);

		VkSemaphore createSemaphore();
		void destroySemaphore(VkSemaphore sem);
		VkFence createFence(bool signalled = false);
		void destroyFence(VkFence fen);
		void waitForFence(VkFence fen);
		void waitForFences(std::vector<VkFence>const& fens);
		void resetFence(VkFence fen);
		VkCommandPool createCommandPool();
		void destroyCommandPool(VkCommandPool pool);
		VkCommandBuffer createCommandBuffer(VkCommandPool pool, bool secondary = false);
		void destroyCommandBuffer(VkCommandBuffer buf, VkCommandPool pool);

		uint64_t minUniformBufferOffsetAlignment();
		uint64_t minStorageBufferOffsetAlignment();
		uint64_t minFlushRangeAlignment();
		ctDynamicOffsetBuffer createDynamicOffsetBuffer(uint64_t size);
		void destroyDynamicOffsetBuffer(ctDynamicOffsetBuffer bm);
		void flushDynamicOffsetBuffer(ctDynamicOffsetBuffer bm);
		void flushDynamicOffsetBufferRange(ctDynamicOffsetBuffer bm, uint64_t start, uint64_t size);

		int swapChainSize();

	public:

		void drawFrame();

		instance(std::string name, GLFWwindow* win, int width, int height, std::string resFolder = "");
		~instance();
	};
}