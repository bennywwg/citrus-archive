#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <util/stdUtil.h>

namespace citrus::graphics {
	using std::string;
	using std::vector;

	class instance {
		friend class QueueFamilyIndices;
		friend class SwapChainSupportDetails;
		friend class vkShader;

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

	public:

		void drawFrame();

		instance(string name, GLFWwindow* win);
		~instance();
	};
}