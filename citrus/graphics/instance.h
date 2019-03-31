#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "citrus/util.h"

namespace citrus::graphics {
	using std::string;
	using std::vector;
	class finalPassShader;

	class instance;

	struct ctFBO {
		VkFramebuffer		fbo;
		VkDescriptorSet		set;
		uint64_t			uboOff; //offset into instance.uniformMem.mem
	};
    
	struct ctTexture {
		VkImage			img;
		VkImageView		view;
		VkSampler		samp;
		uint64_t		off; //offset into instance.textureMem.mem
	};

	struct ctDynamicOffsetBuffer {
		VkBuffer		buf;
		VkDeviceMemory	mem;
		uint64_t		size;
		uint64_t		align;
		void*		mapped;
	};
	
	class fenceProc {
		instance& _inst;
		bool _done = false;

		void _free();
	public:
		VkFence fence = VK_NULL_HANDLE;
		VkCommandBuffer cbuff = VK_NULL_HANDLE;
		uint64_t stagingBuf;

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
		public:
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

		finalPassShader* _finalPass;
        

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
			
			string name;
			instance& inst;
			VkDeviceMemory mem;
			VkBuffer buf;
			uint64_t size;
			uint64_t alignment;
			vector<allocBlock> blocks;
			
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
		void fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, void* data, fenceProc* proc = nullptr);
		void fillBuffer(VkBuffer dstBuffer, uint64_t size, uint64_t start, std::function<void(void*)> fillFunc, fenceProc* proc = nullptr);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size, uint64_t srcStart, uint64_t dstStart, fenceProc* proc = nullptr);
		void copyBufferToImage(VkBuffer stagingBuf, uint64_t start, VkImage image, uint32_t width, uint32_t height, fenceProc* proc = nullptr);
		void pipelineBarrierLayoutChange(VkImage image,
            VkImageLayout oldLayout, VkImageLayout newLayout,
            VkAccessFlags srcAccess, VkAccessFlags dstAccess,
            VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
            fenceProc* proc = nullptr);
		
        //creates an image, image view, and sampler
        //if fboTexture is true, image usage contains VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        //if data is non-null, image usage contains VK_IMAGE_USAGE_TRANSFER_DST_BIT and
        //stage is VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
		ctTexture createTexture4b(uint32_t width, uint32_t height, bool fboTexture, void* data);
		void destroyTexture(ctTexture tex);
		
		//takes ownership of commandBuffer, blocks if proc == nullptr
		void submitFenceProc(VkCommandBuffer commandBuffer, fenceProc* proc = nullptr);

		VkSemaphore createSemaphore();
		void destroySemaphore(VkSemaphore sem);
		VkFence createFence(bool signalled = false);
		void destroyFence(VkFence fen);
		void waitForFence(VkFence fen);
		void resetFence(VkFence fen);
		VkCommandBuffer createCommandBuffer();
		void destroyCommandBuffer(VkCommandBuffer buf);

		uint64_t minUniformBufferOffsetAlignment();
		ctDynamicOffsetBuffer createDynamicOffsetBuffer(uint64_t size);
		void destroyDynamicOffsetBuffer(ctDynamicOffsetBuffer bm);
		void flushDynamicOffsetBuffer(ctDynamicOffsetBuffer bm);
		void flushDynamicOffsetBufferRange(ctDynamicOffsetBuffer bm, uint64_t start, uint64_t size);
		
		int swapChainSize();
		
	private:
		
		
	public:

		void drawFrame();

		instance(string name, GLFWwindow* win, int width, int height, std::string resFolder = "");
		~instance();
	};
}
