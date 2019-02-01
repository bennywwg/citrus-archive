#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <citrus/util.h>

namespace citrus::graphics {
	using std::string;
	using std::vector;
	class vkShader;
	class vkBuffer;

	class instance;

	class fenceProc {
		instance& _inst;
		bool _done = false;

		void _free();
	public:
		VkFence fence = VK_NULL_HANDLE;
		VkCommandBuffer cbuff = VK_NULL_HANDLE;
		VkBuffer sbuf = VK_NULL_HANDLE;
		VkDeviceMemory smem = VK_NULL_HANDLE;

		//checks if fence was signalled
		bool done();

		//blocks execution until the fence is signalled
		void block();

		//recreates a semaphore so this object can be reused
		void reset();

		//creates a fence
		fenceProc(instance& inst);

		//destroys everything that isn't NULL_HANDLE
		~fenceProc();
	};

	class instance {
		friend class QueueFamilyIndices;
		friend class SwapChainSupportDetails;
		friend class vkShader;
		friend class vkBuffer;
		friend class fenceProc;

		const bool enableValidationLayers = true;

		VkInstance _instance;
		vector<VkExtensionProperties> _availableExtensions;
		vector<VkLayerProperties> _availableLayers;
		VkDebugUtilsMessengerEXT _callback;
		vector<VkPhysicalDevice> _availableDevices;
		vector<VkPhysicalDevice> _suitableDevices;
		VkPhysicalDevice _chosenDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;
		VkSurfaceKHR _surface;
		VkSurfaceFormatKHR _surfaceFormat;
		VkPresentModeKHR _presentMode;
		VkExtent2D _extent;
		VkSwapchainKHR _swapChain;
		vector<VkImage> _swapChainImages;
		vector<VkImageView> _swapChainImageViews;
		VkCommandPool _commandPool;
		VkSemaphore _imgAvailableSemaphore, _renderFinishedSemaphore;
		vector<VkDeviceMemory> _memories;
		uint32_t _hostVisibleCoherent;
		uint32_t _deviceLocal;

		vkShader* _finalPass;

		void initInstance(string name);
		void destroyInstance();

		vector<const char*> getRequiredExtensions();
		void loadExtensions();
		void checkExtensions();

		vector<const char*> getRequiredLayers();
		void loadValidationLayers();
		void checkValidationLayers();
		void initDebugCallback();
		void destroyDebugCallback();

		vector<const char*> getRequiredDeviceExtensions();
		bool deviceHasRequiredExtensions(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		void loadPhysicalDevices();
		void choosePhysicalDevice();
		void initDevice();
		void destroyDevice();

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
		
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, uint64_t srcStart, uint64_t dstStart, fenceProc* proc = nullptr);
		
		void fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, const void* data, fenceProc* proc = nullptr);
		void fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, std::function<void(const void*)> fillFunc, fenceProc* proc = nullptr);

		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& img, VkDeviceMemory& mem);
		
		void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, fenceProc* proc = nullptr);

		//takes ownership of commandBuffer, blocks if proc == nullptr
		void submitFenceProc(VkCommandBuffer commandBuffer, fenceProc* proc = nullptr);
		
		VkCommandBuffer createCommandBuffer();
		
	public:

		void drawFrame();

		instance(string name, GLFWwindow* win);
		~instance();
	};
}
