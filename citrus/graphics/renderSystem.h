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
#define animated_vertex_inputs 8
#define static_vertex_inputs 4

namespace citrus::graphics {
	
	class renderPass;

	struct vertexElementArray {
		uint32_t						location;							// location in shader (ie, layout(location = ...) ...)
		uint32_t						binding;							// binding
		VkDeviceAddress					memOffset;							// start of data in vertex memory
	};

	//
	class modelView {
	public:
		vector<vertexElementArray>		elements;							// elements of this model
	};

	class passMaterial {
	public:
		instance& inst;
		VkRenderPass& pass;

		//initializeDescriptorLayouts
		VkDescriptorSetLayout uboLayout;
		VkDescriptorSetLayout texLayout;

		//initializePipelineLayout
		VkPipelineLayout	pipelineLayout;

		//initializeDescriptorSets
		VkDescriptorPool	uboPool;
		VkDescriptorSet		uboSets[SWAP_FRAMES];
		VkDescriptorPool	texPool;
		VkDescriptorSet		texSet;

		//initializePipeline
		VkPipeline			pipeline;

		//
		uint64_t			code;
		inline bool supportsMesh(uint64_t meshCode) {
			return (code & meshCode) == code;
		}

		struct pcData {
			mat4						mvp;
			mat4x3						model;
			uvec4						uints;
		};
		static_assert(sizeof(pcData) == 128, "pcData must be 128 bytes");

		uint8_t *uniformMapped;

		struct threadData {
			uint32_t					offset;		//offset into uniform buffer for this thread
			uint32_t					size;		//size of thread's data
		};
		vector<threadData>				threadRanges;

		/*
		 * represents a set of items to draw
		 * for example, { 1, 4, 68 } represents items 4 through 67 of model index 1
		 */
		struct itemDrawRange {
			int							modelIndex; //index of model
			int							itemBegin;	//first item
			int							itemEnd;	//one past end, ie use <
		};

		/*
		 *	ranges.size() = threadCount
		 *	ranges[n].size() = modelCount
		 */
		vector<vector<itemDrawRange>>	ranges;

		struct itemInfo {
			vec3						pos;
			quat						ori;
			uint32_t					texIndex;
			uint32_t					uniformOffset;
			uint32_t					uniformSize;
			bool						enabled;
		};

		/*
		 *	items.size() = modelCount
		 *	items[n].size() = number of instances of that model
		 */
		vector<vector<itemInfo>>		items;

		passMaterial(renderPass const& pass, vector<string> const& paths);
	};

	class system {
	public:
		instance& inst;

		struct frame {
			VkImage			color;											// color image
			VkImage			depth;											// depth image
			VkImageView		colorView;										// color view
			VkImageView		depthView;										// depth view
			VkSampler		samp;											// color sampler
			VkFramebuffer	frameBuffer;									// fbo
			VkDeviceMemory	colorMem;										// color memory
			VkDeviceMemory	depthMem;										// depth memory
		};
		frame				frames[SWAP_FRAMES];							// frame info

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

		struct model {
			mesh			data;											// CPU mesh object
			VkDeviceAddress	vertexOffsets[animated_vertex_inputs];			// start of memory ranges into vertexBuffer used by vertices
			VkDeviceAddress	vertexSize;										// total size of vertex memory chunk
			VkDeviceAddress indexOffset;									// start of memory range into indexBuffer used by indices
			VkDeviceAddress	indexSize;										// size of index memory chunk
			uint32_t		indexCount;										// number of indices
			float			radius;											// maximal distance any vertex is from origin
		};
		vector<model>		staticModels;									// model info
		vector<model>		aniModels;										// aniModel info
		VkMemoryRequirements vertexRequirements;							// info for vertex memory
		VkDeviceMemory		vertexMemory;									// vertex memory
		VkBuffer			vertexBuffer;									// buffer object associated with vertex memory
		VkBuffer			vertexBuffers[animated_vertex_inputs];			// animated_vertex_inputs copies of vertexBuffer, useful for models and aniModels
		VkMemoryRequirements indexRequirements;								// info for index memory
		VkDeviceMemory		indexMemory;									// index memory
		VkBuffer			indexBuffer;									// buffer object associated with index memory


		VkVertexInputAttributeDescription
							attribs[animated_vertex_inputs];				// description of vertex data types, aniModels use all, models use first 4
		VkVertexInputBindingDescription
							bindings[animated_vertex_inputs];				// bindings of vertex data, aniModels use all, models use first 4

		int					maxItems;
		uint64_t			uniformAlignment;
		VkDeviceMemory		uniformMemories[SWAP_FRAMES];
		VkBuffer			uniformBuffers[SWAP_FRAMES];
		void*				uniformMapped[SWAP_FRAMES];

		struct framebuffer {
			VkImage			img;
			VkImageView		view;
			VkFramebuffer	fbuf;
			VkSampler		samp;
		};

		void createModules(
			string const& vertLoc, string const& fragLoc,
			VkShaderModule& vertModule, VkShaderModule& fragModule,
			VkPipelineShaderStageCreateInfo& vertInfo, VkPipelineShaderStageCreateInfo& fragInfo);

		void initializeAttribsBindings();

		void initializeDescriptorLayouts(int texCount);
		void freeDescriptorLayouts();

		void initializePipelineLayout();
		void freePipelineLayout();

		void initializeDescriptorSets(int texCount);
		void freeDescriptorSets();

		void initializeRenderPass();
		void freeRenderPass();

		void initializePipeline(string vs, string fs);
		void freePipeline();

		void initializeFramebuffers();
		void freeFramebuffers();

		void loadTextures(vector<string> paths);
		void freeTextures();

		void collectModelInfo();
		void loadModels(vector<string> staticPaths, vector<string> aniPaths);
		void initializeModelData();
		void freeModels();

		vector<animation> animations;
		void loadAnimations(vector<string> animations);

		void initializeUniformBuffer();
		void freeUniformBuffer();

		std::shared_mutex		startMut;
		std::mutex				instMut;
		int						frameIndex_ = 0;
		bool					cullEnabled = false;
		camera					frameCam;
		frustrumCullInfo		frameCull;
		mat4					frameVP;
		vector<std::thread>		renderThreads;
		std::atomic_bool		renderGo[32]; //probably never more than 32 threads
		std::atomic_bool		stopped;
		VkFence					waitFences[SWAP_FRAMES];
		VkCommandBuffer			primaryBuffers[SWAP_FRAMES];
		vector<VkCommandPool>	commandPools;
		vector<VkCommandBuffer>	secondaryBuffers[SWAP_FRAMES];
		VkCommandBufferInheritanceInfo inheritanceInfos[SWAP_FRAMES];

		void initializeThreads(int threadCount);
		void freeThreads();


		void sequence(vector<vector<itemInfo>> const& items, vector<vector<itemDrawRange>> & ranges);
		void renderFunc(int threadIndex);
		void renderPartial(
			int threadIndex,
			vector<model> const& models,
			vector<vector<itemInfo>> const& items,
			vector<itemDrawRange> const& ranges,
			VkCommandBuffer & buf);
		void render(int frameIndex, VkSemaphore signal, camera const& cam);
		bool renderDone();

		void postProcess(int frameIndex, int windowSwapIndex, vector<VkSemaphore> waits, VkSemaphore signal);

		void cleanup();

		system(instance & vkinst, string vs, string fs,
			vector<string> textures,
			vector<string> staticModels,
			vector<string> models,
			vector<string> animations);
		~system();
	};
}