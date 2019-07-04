#pragma once

#include <shared_mutex>
#include <atomic>

#include "citrus/graphics/instance.h"
#include "citrus/graphics/image.h"
#include "citrus/graphics/mesh.h"
#include "citrus/graphics/camera.h"
#include "citrus/graphics/finalPassShader.h"

#if false
#define stdioDebug(str) util::sout(str)
#else
#define stdioDebug(str)
#endif

#define SWAP_FRAMES 2

namespace citrus::graphics {
	class system;

	class passBase {
	public:
		system& sys;

		virtual void	preRender(uint32_t const& numThreads) = 0;
		virtual void	renderPartial(uint32_t const& threadIndex) = 0;
		inline			passBase(system& sys) : sys(sys) { }
		virtual			~passBase() = 0;
	};

	class meshPass : public passBase {
	public:
		#pragma region(pipeline stuff)
		string vert, frag;

		VkDescriptorSetLayout	uboLayout;
		VkDescriptorSetLayout	texLayout;
		VkDescriptorPool		uboPool;
		VkDescriptorSet			uboSets;

		VkPipelineLayout		pipelineLayout;
		VkPipeline				pipeline;
		VkRenderPass			pass;
		VkFramebuffer			fbos[SWAP_FRAMES];

		meshUsageLocationMapping meshMappings;
		
		virtual void			initializeDescriptors();
		virtual void			initializePipelineLayout();
		virtual void			initializePipeline();
		virtual void			initializeRenderPass();
		virtual void			initializeFramebuffers();
		virtual void			freePipeline();
		#pragma endergion

		#pragma region(item stuff)
		struct pcData {
			mat4				mvp;
			mat4x3				model;
			uvec4				uints;
		};
		static_assert(sizeof(pcData) == 128, "pcData must be 128 bytes");

		struct itemInfo {
			vec3				pos;
			quat				ori;
			uint32_t			modelIndex;
			uint32_t			texIndex;
			uint32_t			uniformOffset;
			uint32_t			uniformSize;
			bool				enabled;
		};
		vector<itemInfo>		items;

		struct threadData {
			uint32_t			offset;		//offset into uniform buffer for this thread
			uint32_t			size;		//size of thread's data
			uint32_t			begin;		// first item
			uint32_t			end;		// one past end, ie use <
		};
		vector<threadData>		ranges;
		#pragma endregion

		#pragma region(model stuff)

		//mappings to models in system
		struct modelMapping {
			int modelIndex;
			meshMemoryStructure desc;
		};
		vector<modelMapping>	mappings;

		vector<meshAttributeUsage> requiredUsages;

		virtual void			mapModels();
		#pragma endregion

		virtual void preRender(uint32_t const& threadCount);
		virtual void renderPartial(uint32_t const& threadIndex);
		
		meshPass(system & sys, string const& vert, string const& frag);
		~meshPass();
	};

	// the intention is to ultimately replace the model, texture, uniform sections
	// with their own objects so individual system components can
	// maintain their own texture or object or uniform resources
	class system {
	public:
		instance& inst;

		#pragma region(initialization state)
		uint64_t			uniformSize;
		vector<string>		texturePaths;
		vector<string>		modelPaths;
		vector<string>		animationPaths;
		#pragma endregion

		#pragma region(framebuffer stuff)
		struct frame {
			VkImage			color;					// color image
			VkImage			depth;					// depth image
			VkImageView		colorView;				// color view
			VkImageView		depthView;				// depth view
			VkSampler		colorSamp;				// color sampler
			VkSampler		depthSamp;				// color sampler
			VkDeviceMemory	colorMem;				// color memory
			VkDeviceMemory	depthMem;				// depth memory
		};
		frame				frames[SWAP_FRAMES];	// frame info

		void				createFramebufferData();
		void				freeFramebufferData();
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

		void loadTextures();
		void freeTextures();
		#pragma endregion

		#pragma region(model stuff)
		// level of detail information
		struct model {
			mesh			m;												// mesh
			meshMemoryStructure desc;										// mesh description
			float			radius;											// vertex distance farther from origin
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
		VkCommandBuffer			primaryBuffers[SWAP_FRAMES];
		vector<VkCommandPool>	commandPools;
		vector<VkCommandBuffer>	secondaryBuffers[SWAP_FRAMES];
		VkCommandBufferInheritanceInfo inheritanceInfos[SWAP_FRAMES];

		int						frameIndex;		// index of frame  [0, SWAP_FRAMES)

		vector<std::thread>		renderThreads;	// thread objects
		vector<std::atomic_bool>renderGo;		// go?
		std::atomic_bool		stopped;		// halt rendering threads?

		std::mutex				instMut;		// access to vulkan shared functions synchronised with this

		std::shared_mutex		startMut;		// objects below synchronised with this
		camera					frameCam;		// camera
		frustrumCullInfo		frameCull;		// camera cull info cache
		mat4					frameVP;		// projection * view
		#pragma endregion

		#pragma region(uniform stuff)
		uint64_t				uniformAlignment;
		VkDeviceMemory			uniformMemories[SWAP_FRAMES];
		VkBuffer				uniformBuffers[SWAP_FRAMES];
		uint8_t*				uniformMapped[SWAP_FRAMES];

		void					initializeUniformData();
		void					freeUniformData();
		#pragma endregion

	private:
		void				renderDone();
		void				postProcess(int frameIndex, int windowSwapIndex, vector<VkSemaphore> waits, VkSemaphore signal);
	public:

		void				render(VkSemaphore signal);


		system(instance & vkinst,
			vector<string> textures,
			vector<string> models,
			vector<string> animations);
		~system();
	};
}