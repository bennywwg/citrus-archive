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
	class system {
	public:
		instance& inst;

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

		//initializeRenderPass
		VkRenderPass		renderPass;

		//initializePipeline
		VkPipeline			pipeline;

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

		struct dynamicData {
			vec4			nothing;
		};
		static_assert(sizeof(dynamicData) >= 0, "dynamicData must not be zero sized");

		struct vert_pcData {
			mat4			mvp;
			glm::mat4x3		model;
		};
		struct frag_pcData {
			uint32_t		texIndex;
		};
		struct pcData {
			vert_pcData vertData;
			frag_pcData fragData;
		};
		static_assert(sizeof(pcData) <= 128, "pcData must be smaller than push constant min size");

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

		struct itemInfo {
			vec3 pos;
			quat ori;
			int texIndex;
			bool enabled;
		};

		vector<vector<itemInfo>> staticItems;
		vector<vector<itemInfo>> aniItems;

		struct itemDrawRange {
			int modelIndex;
			int itemBegin;
			int itemEnd; //one past end, ie use <
		};
		vector<vector<itemDrawRange>> staticRanges;
		vector<vector<itemDrawRange>> aniRanges;

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