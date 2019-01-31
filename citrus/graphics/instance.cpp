#include <graphics/vulkan/instance.h>
#include <iostream>
#include <cstring>

#ifdef __linux__
#include <experimental/optional>
#elif _WIN32
#include <optional>
#endif

#include <cstdlib>
#include <set>
#include <algorithm>
#include <graphics/vulkan/vkShader.h>

namespace citrus::graphics {
	struct QueueFamilyIndices {
		std::experimental::optional<uint32_t> graphicsFamily;
		std::experimental::optional<uint32_t> presentFamily;

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
		poolInfo.flags = 0; // Optional

		if(vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	void instance::destroyCommandPool() {
		vkDestroyCommandPool(_device, _commandPool, nullptr);
	}

	void instance::initSemaphores() {
		VkSemaphoreCreateInfo semaphoreInfo = {};
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

	void instance::drawFrame() {
		uint32_t imageIndex;
		vkAcquireNextImageKHR(_device, _swapChain, std::numeric_limits<uint64_t>::max(), _imgAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {_imgAvailableSemaphore};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_finalPass->_buffers[imageIndex];
		VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {_swapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(_presentQueue, &presentInfo);
		vkQueueWaitIdle(_presentQueue);
	}

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
	void instance::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(_device, buffer, bufferMemory, 0);
	}
	void instance::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, uint64_t srcStart, uint64_t dstStart, fenceProc* proc) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

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
	
	void instance::fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, const void* data, fenceProc* proc) {
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;
		createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);
		
		void* scratch;
		vkMapMemory(_device, stagingMemory, start, size, 0, &scratch);
		memcpy(scratch, data, size);
		vkUnmapMemory(_device, stagingMemory);

		if(proc) {
			if(proc->sbuf != VK_NULL_HANDLE || proc->smem != VK_NULL_HANDLE) throw std::runtime_error("proc already contains staging buffer or staging memory");
			proc->sbuf = stagingBuffer;
			proc->smem = stagingMemory;
			copyBuffer(stagingBuffer, dstBuffer, size, 0, start, proc);
		} else {
			copyBuffer(stagingBuffer, dstBuffer, size, 0, start, nullptr); //this will block because proc is nullptr
			vkDestroyBuffer(_device, stagingBuffer, nullptr);
			vkFreeMemory(_device, stagingMemory, nullptr);
		}
	}
	void instance::fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, std::function<void(const void*)> fillFunc, fenceProc* proc) {
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;
		createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);
		
		void* scratch;
		vkMapMemory(_device, stagingMemory, start, size, 0, &scratch);
		fillFunc(scratch);
		vkUnmapMemory(_device, stagingMemory);

		if(proc) {
			if(proc->sbuf != VK_NULL_HANDLE || proc->smem != VK_NULL_HANDLE) throw std::runtime_error("proc already contains staging buffer or staging memory");
			proc->sbuf = stagingBuffer;
			proc->smem = stagingMemory;
			copyBuffer(stagingBuffer, dstBuffer, size, 0, start, proc);
		} else {
			copyBuffer(stagingBuffer, dstBuffer, size, 0, start, nullptr); //this will block because proc is nullptr
			vkDestroyBuffer(_device, stagingBuffer, nullptr);
			vkFreeMemory(_device, stagingMemory, nullptr);
		}
	}
	void instance::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, fenceProc* proc) {
		VkCommandBuffer commandBuffer = createCommandBuffer();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::runtime_error("unsupported layout transition!");
        }
		
		VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
		
		vkEndCommandBuffer(commandBuffer);
		
		submitFenceProc(commandBuffer, proc);
	}
	
	void instance::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory) {
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;3
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

		if (vkCreateImage(_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(_device, image, memory, 0);
	}
	
	void instance::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, fenceProc* proc) {
		VkCommandBuffer commandBuffer = createCommandBuffer();
		
		VkBufferImageCopy region = { };
		region.bufferOffset = 0;
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
		
		submitFenceProc(commandBuffer, proc);
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

	instance::instance(string name, GLFWwindow* win) {
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
		chooseSurfaceExtent(640,480);
		initSwapChain();
		initCommandPool();
		initSemaphores();
		_finalPass = new vkShader(*this,
			_swapChainImageViews,
			640, 480,
			"/home/benny/Desktop/folder/citrus/res/shaders/vert.spv",
			"",
			"/home/benny/Desktop/folder/citrus/res/shaders/frag.spv");
		_finalPass->beginAll();
		_finalPass->drawAll(3);
		_finalPass->endAll();
		VkMemoryPropertyFlags props;
	}
	instance::~instance() {
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
		if(sbuf != VK_NULL_HANDLE) vkDestroyBuffer(_inst._device, sbuf, nullptr);
		if(smem != VK_NULL_HANDLE) vkFreeMemory(_inst._device, smem, nullptr);
		fence = VK_NULL_HANDLE;
		cbuff = VK_NULL_HANDLE;
		sbuf = VK_NULL_HANDLE;
		smem = VK_NULL_HANDLE;
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