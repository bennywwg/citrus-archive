#pragma once

#include <shared_mutex>
#include <atomic>

#include "citrus/graphics/system/runtimeResource.h"
#include "citrus/graphics/image.h"
#include "citrus/graphics/mesh/mesh.h"
#include "citrus/graphics/camera.h"
#include "citrus/util.h"

#if false
#define stdioDebug(str) util::sout(str)
#else
#define stdioDebug(str)
#endif

namespace citrus::graphics {
	class system;

	enum class passType : uint32_t {
		nonePass =		0,
		drawPass =		1 << 0,
		postPass =		1 << 1,
		computePass =	1 << 2
	};

	class sysNode;

	class meshPass;

	// the intention is to ultimately replace the model, texture, uniform sections
	// with their own objects so individual system components can
	// maintain their own texture or object or uniform resources
	class system {
	public:
		instance& inst;

		#pragma region(initialization state)
		vector<fpath>		texturePaths;
		vector<fpath>		modelPaths;
		vector<fpath>		animationPaths;
		#pragma endregion

		#pragma region(texture stuff)
		struct texture {
			image4b			data;											// CPU image object
			VkFormat		format;											// format info
			VkImage			img;											// image object
			VkImageView		view;											// view object
			VkSampler		samp;											// sampler object
			VkDeviceAddress	off;											// start of memory range into textureMem
		};
		vector<texture>		textures;										// texture info
		VkDeviceMemory		textureMem;										// texture memory
		VkDescriptorPool	texPool;										// pool for allocating texture descriptors
		VkDescriptorSet		texSet;											// set of textures.size()

		cubemapStore		cubemaps;

		void loadTextures();
		void freeTextures();
		#pragma endregion

		#pragma region(model stuff)
		struct model {
			mesh			m;												// mesh
			meshMemoryStructure desc;										// mesh description
			float			radius;											// vertex distance farther from origin
			fpath			source;											// from whence 'twas loaded
		};

		vector<model>		models;											// model info
		vector<animation>	animations;										// animations (no GPU)

		VkMemoryRequirements vertexRequirements;							// info for vertex memory
		VkDeviceMemory		vertexMemory;									// vertex memory
		VkBuffer			vertexBuffer;									// buffer object associated with vertex memory
		VkBuffer			vertexBuffers[16];								// a bunch of copies of vertexBuffer needed by vulkan
		VkMemoryRequirements indexRequirements;								// info for index memory
		VkDeviceMemory		indexMemory;									// index memory
		VkBuffer			indexBuffer;									// buffer object associated with index memory

		void				collectModelInfo();
		void				loadModels();
		void				initializeModelData();
		void				freeModels();

		void				loadAnimations();
		#pragma endregion

		#pragma region(rendering stuff)
		vector<VkCommandPool>	commandPools;

		int						frameIndex;		// index of frame  [0, SWAP_FRAMES)

		vector<std::thread>		renderThreads;	// thread objects
		vector<std::atomic_bool>renderGo;		// go?
		std::atomic_bool		stopped;		// halt rendering threads?

		std::mutex				instMut;		// access to vulkan shared functions synchronised with this

		std::shared_mutex		startMut;		// objects below synchronised with this
		uint32_t				currentPass;	// which pass is currently being invoked (index into passes)
		camera					frameCam;		// camera
		frustrumCullInfo		frameCull;		// camera cull info cache
		mat4					frameVP;		// projection * view
		#pragma endregion

		#pragma region(rendertime stuff)
	private:
		
	public:
		#pragma endregion
	private:
		void				postProcess(int frameIndex, int windowSwapIndex, vector<VkSemaphore> waits, VkSemaphore signal);
	public:
		vector<sysNode*>	passes;

		void				initializeThreads(uint32_t numThreads);
		void				freeThreads();
		bool				renderDone() const;
		void				renderFunc(uint32_t threadIndex);

		void				render();

		vector<meshPass*>	meshPasses;

		system(instance & vkinst,
			fpath texturePath,
			fpath modelPath,
			fpath animationPath);
		~system();
	};
}