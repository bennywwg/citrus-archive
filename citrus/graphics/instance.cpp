#include "citrus/graphics/instance.h"
#include <iostream>
#include <cstring>

#ifdef __linux__
#include <experimental/optional>
template<typename T>
using optional_t = std::experimental::optional<T>;
#elif _WIN32
#include <optional>
template<typename T>
using optional_t = std::optional<T>;
#endif

#include <cstdlib>
#include <set>
#include <algorithm>
#include "citrus/graphics/finalPassShader.h"

namespace citrus::graphics {
	struct QueueFamilyIndices {
		optional_t<uint32_t> graphicsFamily;
		optional_t<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily && presentFamily;
		}

		QueueFamilyIndices(VkPhysicalDevice device, instance* inst) {
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
			vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for(const auto& queueFamily : queueFamilies) {
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, inst->_surface, &presentSupport);

				if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					graphicsFamily = i;
				}

				if(queueFamily.queueCount > 0 && presentSupport) {
					presentFamily = i;
				}

				if(isComplete()) {
					break;
				}

				i++;
			}
		}
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		vector<VkSurfaceFormatKHR> formats;
		vector<VkPresentModeKHR> presentModes;

		SwapChainSupportDetails(VkPhysicalDevice device, instance* inst) {
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, inst->_surface, &capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, inst->_surface, &formatCount, nullptr);

			if(formatCount != 0) {
				formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, inst->_surface, &formatCount, formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, inst->_surface, &presentModeCount, nullptr);

			if(presentModeCount != 0) {
				presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, inst->_surface, &presentModeCount, presentModes.data());
			}
		}
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT || messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		}


		return VK_FALSE;
	}
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if(func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pCallback);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if(func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}

	void instance::initInstance(string name) {
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = name.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.pEngineName = "citrus";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;


		const auto& extensions = getRequiredExtensions();
		const auto& layers = getRequiredLayers();

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		if(enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			createInfo.ppEnabledLayerNames = layers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}



		VkResult res = vkCreateInstance(&createInfo, nullptr, &_instance);
		if(res != VK_SUCCESS) throw std::runtime_error("vkCreateInstance failure");
	}
	void instance::destroyInstance() {
		vkDestroyInstance(_instance, nullptr);
	}

	vector<const char*> instance::getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if(enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}
	void instance::loadExtensions() {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		_availableExtensions = vector<VkExtensionProperties>(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, _availableExtensions.data());
	}
	void instance::checkExtensions() {
		vector<const char*> required = getRequiredExtensions();
		for(const char* extensionName : required) {
			bool found = false;
			for(const auto& extensionProperties : _availableExtensions)
				if(string(extensionName) == string(extensionProperties.extensionName)) {
					found = true;
					break;
				}

			if(!found) throw std::runtime_error(("Extension \"" + string(extensionName) + "\" not available").c_str());
		}
	}

	vector<const char*> instance::getRequiredLayers() {
		return vector<const char*> {
			"VK_LAYER_LUNARG_standard_validation"
		};
	}
	void instance::loadValidationLayers() {
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		_availableLayers = vector<VkLayerProperties>(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, _availableLayers.data());

		
	}
	void instance::checkValidationLayers() {
		for(const char* layerName : getRequiredLayers()) {
			bool found = false;
			for(const auto& layerProperties : _availableLayers)
				if(string(layerName) == string(layerProperties.layerName)) {
					found = true;
					break;
				}

			if(!found) throw std::runtime_error(("Layer \"" + string(layerName) + "\" not available").c_str());
		}
	}
	void instance::initDebugCallback() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;

		VkResult res = CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_callback);
		if(res != VK_SUCCESS)
			throw std::runtime_error("CreateDebugUtilsMessengerEXT failed");
	}
	void instance::destroyDebugCallback() {
		DestroyDebugUtilsMessengerEXT(_instance, _callback, nullptr);
	}

	vector<const char*> instance::getRequiredDeviceExtensions() {
		return vector<const char*> {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	}
	bool instance::deviceHasRequiredExtensions(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		auto requiredDeviceExtensions = getRequiredDeviceExtensions();
		std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

		for(const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
	bool instance::isDeviceSuitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		auto swapChainDetails = SwapChainSupportDetails(device, this);

		return deviceHasRequiredExtensions(device) &&
			!swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
			deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader;
	}
	void instance::loadPhysicalDevices() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
		_availableDevices = vector<VkPhysicalDevice>(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, _availableDevices.data());
		for(auto& device : _availableDevices) if(isDeviceSuitable(device)) _suitableDevices.push_back(device);
	}
	void instance::choosePhysicalDevice() {
		uint32_t best = 0;
		VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
		for(auto& device : _suitableDevices) {
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(device, &props);
			if(props.limits.maxImageDimension2D > best) {
				best = props.limits.maxImageDimension2D;
				bestDevice = device;
			}
		}
		_chosenDevice = bestDevice;
		if(_chosenDevice == VK_NULL_HANDLE) throw std::runtime_error("No suitable device found");
	}
	void instance::initDevice() {
		QueueFamilyIndices indices(_chosenDevice, this);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		float queuePriority = 1.0f;

		for(uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		auto requiredDeviceExtensions = getRequiredDeviceExtensions();
		auto layers = getRequiredLayers();

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
		if(enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			createInfo.ppEnabledLayerNames = layers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		VkResult res = vkCreateDevice(_chosenDevice, &createInfo, nullptr, &_device);
		if(res != VK_SUCCESS) throw std::runtime_error("Could not creat device");

		vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
	}
	void instance::destroyDevice() {
		vkDestroyDevice(_device, nullptr);
	}
	
	VkFormat instance::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(_chosenDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }
    VkFormat instance::findDepthFormat() {
        return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

	void instance::initSurface(GLFWwindow* win) {
		VkResult res = glfwCreateWindowSurface(_instance, win, nullptr, &_surface);
		if(res != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface!");
	}
	void instance::destroySurface() {
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	}
	void instance::chooseSurfaceFormat() {
		auto swapChainDetails = SwapChainSupportDetails(_chosenDevice, this);

		if(swapChainDetails.formats.size() == 1 && swapChainDetails.formats[0].format == VK_FORMAT_UNDEFINED) {
			_surfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
		}

		for(const auto& availableFormat : swapChainDetails.formats) {
			if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				_surfaceFormat = availableFormat;
			}
		}

		_surfaceFormat = swapChainDetails.formats[0];
	}
	void instance::chooseSurfacePresentMode() {
		auto swapChainDetails = SwapChainSupportDetails(_chosenDevice, this);

		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for(const auto& availablePresentMode : swapChainDetails.presentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				_presentMode = availablePresentMode;
				return;
			} else if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				bestMode = availablePresentMode;
			}
		}

		_presentMode = bestMode;
	}
	void instance::chooseSurfaceExtent(uint32_t width, uint32_t height) {
		auto swapChainDetails = SwapChainSupportDetails(_chosenDevice, this);

		if(swapChainDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			_extent = swapChainDetails.capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent = {width, height};

			actualExtent.width = std::max(swapChainDetails.capabilities.minImageExtent.width, std::min(swapChainDetails.capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(swapChainDetails.capabilities.minImageExtent.height, std::min(swapChainDetails.capabilities.maxImageExtent.height, actualExtent.height));

			_extent = actualExtent;
		}
	}
	void instance::initSwapChain() {
		auto swapChainDetails = SwapChainSupportDetails(_chosenDevice, this);

		uint32_t imageCount = swapChainDetails.capabilities.minImageCount + 1;
		if(swapChainDetails.capabilities.maxImageCount > 0 && imageCount > swapChainDetails.capabilities.maxImageCount) {
			imageCount = swapChainDetails.capabilities.maxImageCount;
		}

		auto indices = QueueFamilyIndices(_chosenDevice, this);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = _surfaceFormat.format;
		createInfo.imageColorSpace = _surfaceFormat.colorSpace;
		createInfo.imageExtent = _extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if(indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		createInfo.preTransform = swapChainDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = _presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult res = vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain);
		//if(res != VK_SUCCESS) throw std::runtime_error("Failed to create swapchain");

		vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
		_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

		_swapChainImageViews.resize(_swapChainImages.size());
		for(size_t i = 0; i < _swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _surfaceFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult res = vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]);
			if(res != VK_SUCCESS) throw std::runtime_error("Failed to create swapchain image view");
		}
	}
	void instance::destroySwapChain() {
		for(auto imageView : _swapChainImageViews) {
			vkDestroyImageView(_device, imageView, nullptr);
		}
		vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	}
	void instance::initFrameBuffers() {
		/*_frameBuffers.resize(_swapChainImageViews.size());

		for(size_t i = 0; i < _swapChainImageViews.size(); i++) {
			VkImageView attachments[] = {
				_swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = ;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}*/
	}

	void instance::initCommandPool() {
		QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices(_chosenDevice, this);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

		if(vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	void instance::destroyCommandPool() {
		vkDestroyCommandPool(_device, _commandPool, nullptr);
	}

	void instance::initSemaphores() {
		VkSemaphoreCreateInfo semaphoreInfo = { };
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imgAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}
	void instance::destroySemaphores() {
		vkDestroySemaphore(_device, _imgAvailableSemaphore, nullptr);
		vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
	}

	int num = 0;

    //sequence of events
    //query window for next frame index and setup semaphore to signal when that image is available
    //submit final pass shader, waiting for image available, and signalling when done rendering
    //present rendered frame, waiting for when done rendering
	void instance::drawFrame() {

	}
	
	void instance::initMemory() {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(_chosenDevice, &props);
		vertexMem.initBuffer(256 * 1024 * 1024, 0,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vertexMem.name = "vertex";
		indexMem.initBuffer(64 * 1024 * 1024, 0,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexMem.name = "index";
		uniformMem.initBuffer(16 * 1024 * 1024, props.limits.minUniformBufferOffsetAlignment,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		uniformMem.name = "uniform";
		textureMem.initImage(512 * 1024 * 1024, 0,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		textureMem.name = "texture";
        fboMem.initImage(64 * 1024 * 1024, 0, //enough for nearly 8 1080p framebuffers
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        fboMem.name = "framebuffer";
		stagingMem.initBuffer(16 * 1024 * 1024, 0,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingMem.name = "staging";
	}
	
	uint64_t instance::allocator::alloc(uint64_t size) {
		//util::sout(name + " allocating " + std::to_string(size) + " bytes... \n");
		//util::sout("#blocks = " + std::to_string(blocks.size()) + "\n");
		for(int i = 0; i <= blocks.size(); i++) {
			uint64_t addr = (i == 0) ? 0 : (blocks[i - 1].addr + blocks[i - 1].size);
            if(alignment && addr % alignment) addr += alignment - (addr % alignment);
			uint64_t nend = (i == blocks.size()) ? this->size : blocks[i].addr;
			//util::sout("\tscan: addr = " + std::to_string(addr) + ", nend = " + std::to_string(nend) + "\n");
			if(addr + size <= nend) {
				blocks.push_back({ addr, size });
				//util::sout("got " + std::to_string(addr) + "\n");
				return addr;
			}
		}
		throw std::runtime_error(("out of " + name + " memory").c_str());
	}
	void instance::allocator::free(uint64_t addr) {
		for(int i = 0; i < blocks.size(); i++) {
			if(addr == blocks[i].addr) {
				blocks.erase(blocks.begin() + i);
				return;
			}
		}
		throw std::runtime_error("can't find block to free");
	}
	void instance::allocator::initBuffer(uint64_t size, uint64_t align, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		this->size = size;
        this->alignment = align;
		
		VkBufferCreateInfo bufferInfo = { };
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		if (vkCreateBuffer(inst._device, &bufferInfo, nullptr, &buf) != VK_SUCCESS) {
			throw std::runtime_error("failed to create memory buffer!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(inst._device, buf, &memRequirements);
		
		VkMemoryAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = inst.findMemoryType(memRequirements.memoryTypeBits, properties);
		
		if (vkAllocateMemory(inst._device, &allocInfo, nullptr, &mem) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}
		
		vkBindBufferMemory(inst._device, buf, mem, 0);
	}
	void instance::allocator::initImage(uint64_t size, uint64_t align, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		this->size = size;
        this->alignment = align;
		this->buf = VK_NULL_HANDLE;

		VkImage tmpImage;

		VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = 1;
        imageInfo.extent.height = 1;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(inst._device, &imageInfo, nullptr, &tmpImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

		VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(inst._device, tmpImage, &memRequirements);

		vkDestroyImage(inst._device, tmpImage, nullptr);
		
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = size;
		allocInfo.memoryTypeIndex = inst.findMemoryType(memRequirements.memoryTypeBits, properties);
		
		if (vkAllocateMemory(inst._device, &allocInfo, nullptr, &mem) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}
	}
	void instance::allocator::freeResources() {
		if(buf != VK_NULL_HANDLE) vkDestroyBuffer(inst._device, buf, nullptr);
		vkFreeMemory(inst._device, mem, nullptr);
	}
	instance::allocator::allocator(instance& inst) : inst(inst) { }
	
	/*img instance::createImg(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkBuffer im;
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(_device, &imageInfo, nullptr, &im) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, img, &memRequirements);
		uint64_t start = _imMem.alloc(memRequirements.size);

		vkBindImageMemory(_device, im, _imMemory, start);

		img res = { };
		res.img = im;
		res.start = start;
		return res;
	}
	void instance::freeImg(img const& im) {
		vkDestroyImage(_device, im, nullptr);
		_imMem.free(im.start);
	}*/

	uint32_t instance::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_chosenDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			//std::cout << "mem: " << std::dec << i << ", propFlags = " << std::hex << memProperties.memoryTypes[i].propertyFlags << "\n";
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
	void instance::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size, uint64_t srcStart, uint64_t dstStart, fenceProc* proc) {
		VkCommandBuffer commandBuffer = createCommandBuffer();
		
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = srcStart;
		copyRegion.dstOffset = dstStart;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);
		
		submitFenceProc(commandBuffer, proc);
	}
	
	void instance::mapUnmapMemory(VkDeviceMemory dstMemory, uint64_t size, uint64_t start, void* data) {
		void* scratch;
		vkMapMemory(_device, dstMemory, start, size, 0, &scratch);
		memcpy(scratch, data, size);
		vkUnmapMemory(_device, dstMemory);
	}
	void instance::fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, void* data, fenceProc* proc) {
		uint64_t stagingBuffer = stagingMem.alloc(size);
		
		void* scratch;
		vkMapMemory(_device, stagingMem.mem, stagingBuffer + start, size, 0, &scratch);
		memcpy(scratch, data, size);
		vkUnmapMemory(_device, stagingMem.mem);

		if(proc) {
			if(proc->stagingBuf != VK_NULL_HANDLE) throw std::runtime_error("proc already contains staging buffer or staging memory");
			proc->stagingBuf = stagingBuffer;
			copyBuffer(stagingMem.buf, dstBuffer, size, 0, start, proc);
		} else {
			copyBuffer(stagingMem.buf, dstBuffer, size, 0, start, nullptr); //this will block because proc is nullptr
			stagingMem.free(stagingBuffer);
		}
	}
	void instance::fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, std::function<void(void*)> fillFunc, fenceProc* proc) {
		uint64_t stagingBuffer = stagingMem.alloc(size);
		
		void* scratch;
		vkMapMemory(_device, stagingMem.mem, start + stagingBuffer, size, 0, &scratch);
		fillFunc(scratch);
		vkUnmapMemory(_device, stagingMem.mem);

		if(proc) {
			if(proc->stagingBuf != VK_NULL_HANDLE) throw std::runtime_error("proc already contains staging buffer or staging memory");
			proc->stagingBuf = stagingBuffer;
			copyBuffer(stagingMem.buf, dstBuffer, size, 0, start, proc);
		} else {
			copyBuffer(stagingMem.buf, dstBuffer, size, 0, start, nullptr); //this will block because proc is nullptr
			stagingMem.free(stagingBuffer);
		}
	}
	
	void instance::copyBufferToImage(VkBuffer buffer, uint64_t start, VkImage image, uint32_t width, uint32_t height, fenceProc* proc) {
		VkCommandBuffer commandBuffer = createCommandBuffer();
		
		VkBufferImageCopy region = { };
		region.bufferOffset = start;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };
		
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
        
        vkEndCommandBuffer(commandBuffer);
		
		submitFenceProc(commandBuffer, proc);
	}
	void instance::pipelineBarrierLayoutChange(VkImage image,
        VkImageLayout oldLayout, VkImageLayout newLayout,
        VkAccessFlags srcAccess, VkAccessFlags dstAccess,
        VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
        fenceProc* proc) {
		VkCommandBuffer commandBuffer = createCommandBuffer();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcAccessMask = srcAccess;
        barrier.dstAccessMask = dstAccess;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
		
		VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        vkCmdPipelineBarrier(
            commandBuffer,
            srcStage, dstStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
		
		vkEndCommandBuffer(commandBuffer);
		
		submitFenceProc(commandBuffer, proc);
	}
	
	ctTexture instance::createTexture4b(uint32_t width, uint32_t height, bool fboTexture, void *data) {
		VkImageCreateInfo imageInfo = { };
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage =
            (data ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0) |
            (fboTexture ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0) |
            VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		
		VkImage img;
		VkImageView view;
		VkSampler samp;
		
		if (vkCreateImage(_device, &imageInfo, nullptr, &img) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, img, &memRequirements);


		uint64_t off = textureMem.alloc(memRequirements.size);
		vkBindImageMemory(_device, img, textureMem.mem, off);

		if (data) {
			uint64_t tmp = stagingMem.alloc(width * height * 4);
			mapUnmapMemory(stagingMem.mem, width * height * 4, tmp, data);
			pipelineBarrierLayoutChange(img,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0, VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
				nullptr);
			copyBufferToImage(stagingMem.buf, tmp, img, width, height, nullptr);
			pipelineBarrierLayoutChange(img,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				nullptr);
			stagingMem.free(tmp);
		}
		
		VkImageViewCreateInfo viewInfo = { };
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = img;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		
		if (vkCreateImageView(_device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
		
		VkSamplerCreateInfo samplerInfo = { };
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		
		
		if (vkCreateSampler(_device, &samplerInfo, nullptr, &samp) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
		
		ctTexture res = { };
		res.img = img;
		res.view = view;
		res.samp = samp;
		res.off = off;
		
		return res;
	}
	void instance::destroyTexture(ctTexture tex) {
		vkDestroySampler(_device, tex.samp, nullptr);
		vkDestroyImageView(_device, tex.view, nullptr);
		vkDestroyImage(_device, tex.img, nullptr);
	}
	
	void instance::submitFenceProc(VkCommandBuffer commandBuffer, fenceProc* proc) {
		if(proc && (proc->fence != VK_NULL_HANDLE  || proc->cbuff != VK_NULL_HANDLE)) std::runtime_error("proc already contains a fence or command buffer");
		
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		
		VkFenceCreateInfo info = { };
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		if(proc) {
			proc->cbuff = commandBuffer;
			vkCreateFence(_device, &info, nullptr, &proc->fence);
			vkQueueSubmit(_graphicsQueue, 1, &submitInfo, proc->fence);
		} else {
			VkFence tmpFence;
			vkCreateFence(_device, &info, nullptr, &tmpFence);
			vkQueueSubmit(_graphicsQueue, 1, &submitInfo, tmpFence);
			vkWaitForFences(_device, 1, &tmpFence, true, std::numeric_limits<uint64_t>::max());
			vkDestroyFence(_device, tmpFence, nullptr);
			vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
		}
	}

	VkSemaphore instance::createSemaphore() {
		VkSemaphore res = VK_NULL_HANDLE;

		VkSemaphoreCreateInfo info = { };
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.flags = 0;
		if (vkCreateSemaphore(_device, &info, nullptr, &res) != VK_SUCCESS) throw std::runtime_error("couldn't create semaphore");

		return res;
	}
	void instance::destroySemaphore(VkSemaphore sem) {
		vkDestroySemaphore(_device, sem, nullptr);
	}
	VkFence instance::createFence(bool signalled) {
		VkFence fen;

		VkFenceCreateInfo info = { };
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = signalled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		if(vkCreateFence(_device, &info, nullptr, &fen) != VK_SUCCESS) throw std::runtime_error("couldn't create fence");
		return fen;
	}
	void instance::destroyFence(VkFence fen) {
		vkDestroyFence(_device, fen, nullptr);
	}
	void instance::waitForFence(VkFence fen) {
		if(vkWaitForFences(_device, 1, &fen, true, 0) != VK_SUCCESS) throw std::runtime_error("failed to wait for fence");
	}
	void instance::resetFence(VkFence fen) {
		if (vkResetFences(_device, 1, &fen) != VK_SUCCESS) throw std::runtime_error("failed to reset fence");
	}
	
	int instance::swapChainSize() {
		return _swapChainImages.size();
	}

	VkCommandBuffer instance::createCommandBuffer() {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);
		
        return commandBuffer;
    }
	void instance::destroyCommandBuffer(VkCommandBuffer buf) {
		vkFreeCommandBuffers(_device, _commandPool, 1, &buf);
	}

	uint64_t instance::minUniformBufferOffsetAlignment() {
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(_chosenDevice, &props);
		return props.limits.minUniformBufferOffsetAlignment;
	}
	ctDynamicOffsetBuffer instance::createDynamicOffsetBuffer(uint64_t size) {

		ctDynamicOffsetBuffer res = { };
		res.size = size;
		res.align = minUniformBufferOffsetAlignment();

		VkBufferCreateInfo bufferInfo = { };
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &bufferInfo, nullptr, &res.buf) != VK_SUCCESS) {
			throw std::runtime_error("failed to create memory buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, res.buf, &memRequirements);

		VkMemoryAllocateInfo allocInfo = { };
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &res.mem) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(_device, res.buf, res.mem, 0);

		vkMapMemory(_device, res.mem, 0, res.size, 0, &res.mapped);
		
		return res;
	}
	void instance::destroyDynamicOffsetBuffer(ctDynamicOffsetBuffer bm) {
		vkUnmapMemory(_device, bm.mem);
		vkDestroyBuffer(_device, bm.buf, nullptr);
		vkFreeMemory(_device, bm.mem, nullptr);
	}

	void instance::flushDynamicOffsetBuffer(ctDynamicOffsetBuffer bm) {
		VkMappedMemoryRange range = { };
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.memory = bm.mem;
		range.offset = 0;
		range.size = bm.size;

		vkFlushMappedMemoryRanges(_device, 1, &range);
	}

	void instance::flushDynamicOffsetBufferRange(ctDynamicOffsetBuffer bm, uint64_t start, uint64_t size) {
		VkMappedMemoryRange range = { };
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.memory = bm.mem;
		range.offset = start;
		range.size = size;

		vkFlushMappedMemoryRanges(_device, 1, &range);
	}
	
	instance::instance(string name, GLFWwindow* win, int width, int height, std::string resFolder) :
	vertexMem(*this), indexMem(*this), uniformMem(*this), textureMem(*this), fboMem(*this), stagingMem(*this) {
		loadExtensions();
		checkExtensions();
		loadValidationLayers();
		if(enableValidationLayers) checkValidationLayers();
		initInstance(name);
		if(enableValidationLayers)	initDebugCallback();
		initSurface(win);
		loadPhysicalDevices();
		choosePhysicalDevice();
		initDevice();
		chooseSurfaceFormat();
		chooseSurfacePresentMode();
		chooseSurfaceExtent(width,height);
		initSwapChain();
		initCommandPool();
		initSemaphores();
		initMemory();
		_finalPass = new finalPassShader(*this,
			_swapChainImageViews,
			width, height,
			resFolder + "/shaders/finalpass.vert.spv",
			resFolder + "/shaders/finalpass.frag.spv");
	}
	instance::~instance() {
		vertexMem.freeResources();
		indexMem.freeResources();
		uniformMem.freeResources();
		textureMem.freeResources();
        fboMem.freeResources();
		stagingMem.freeResources();

		delete _finalPass;
		destroySemaphores();
		destroyCommandPool();
		destroySwapChain();
		destroySurface();
		destroyDevice();
		if(enableValidationLayers) destroyDebugCallback();
		destroyInstance();
	}

	void fenceProc::_free() {
		if(!_done) throw std::runtime_error("buffer process destroyed before fence signaled (or before done was ever checked)");
		if(fence != VK_NULL_HANDLE) vkDestroyFence(_inst._device, fence, nullptr);
		if(cbuff != VK_NULL_HANDLE) vkFreeCommandBuffers(_inst._device, _inst._commandPool, 1, &cbuff);
		if(stagingBuf != -1) _inst.stagingMem.free(stagingBuf);
		fence = VK_NULL_HANDLE;
		cbuff = VK_NULL_HANDLE;
		stagingBuf = -1;
	}

	bool fenceProc::done() {
		if(_done) return true;
		_done = vkWaitForFences(_inst._device, 1, &fence, true, 0) == VK_EVENT_SET;
		if(_done) {
			_free();
		}
	}

	void fenceProc::reset() {
		_free();
		_done = false;
	}

	void fenceProc::block() {
		if(fence == VK_NULL_HANDLE) throw std::runtime_error("fence was never set");
		if(_done) return;
		vkWaitForFences(_inst._device, 1, &fence, true, std::numeric_limits<uint64_t>::max());
		_done = true;
		_free();
	}

	fenceProc::fenceProc(instance& inst) : _inst(inst) {
	}
	fenceProc::~fenceProc() {
		_free();
	}
}
