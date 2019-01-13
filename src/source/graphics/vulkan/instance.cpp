#include <graphics/vulkan/instance.h>
#include <iostream>
#include <optional>
#include <set>
#include <algorithm>
#include <graphics/vulkan/vkShader.h>

namespace citrus::graphics {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
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

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

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
				if(strcmp(extensionName, extensionProperties.extensionName) == 0) {
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
				if(strcmp(layerName, layerProperties.layerName) == 0) {
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
	void instance::initCommandBuffers() {
		/*_commandBuffers.resize(_swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for(size_t i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = swapChainExtent;

			VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}*/
	}

	void instance::initSemaphores() {
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imgAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
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
		submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
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
	}
	instance::~instance() {
		destroySwapChain();
		destroySurface();
		destroyDevice();
		if(enableValidationLayers) destroyDebugCallback();
		destroyInstance();
	}
}