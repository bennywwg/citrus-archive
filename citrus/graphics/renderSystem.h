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
#define RENDER_VERTEX_INPUTS 8

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
			VkImage			color;
			VkImage			depth;
			VkImageView		colorView;
			VkImageView		depthView;
			VkSampler		samp;
			VkFramebuffer	frameBuffer;
			VkDeviceMemory	colorMem;
			VkDeviceMemory	depthMem;
		};
		frame				frames[SWAP_FRAMES];

		struct texture {
			image4b			data;
			VkFormat		format;
			VkImage			img;
			VkImageView		view;
			VkSampler		samp;
			uint64_t		off;
		};
		vector<texture>		textures;
		VkDeviceMemory		textureMem;

		struct model {
			mesh			data;
			VkDeviceAddress	vertexOffsets[RENDER_VERTEX_INPUTS];
			VkDeviceAddress indexOffset;
			uint32_t		indexCount;
			float			radius;
		};
		vector<model>		models;
		VkDeviceMemory		vertexMemory;
		VkBuffer			vertexBuffer;
		VkBuffer			vertexBuffers[RENDER_VERTEX_INPUTS];  // RENDER_VERTEX_INPUTS copies of vertexBuffer
		VkDeviceMemory		indexMemory;
		VkBuffer			indexBuffer;

		VkVertexInputAttributeDescription	attribs[RENDER_VERTEX_INPUTS];
		VkVertexInputBindingDescription		bindings[RENDER_VERTEX_INPUTS];

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

		void loadModels(vector<string> paths);
		void freeModels();

		void loadAnimations(vector<string> animations);

		void initializeUniformBuffer();
		void freeUniformBuffer();

		struct itemInfo {
			vec3 pos;
			quat ori;
			int texIndex;
			bool enabled;
		};

		vector<vector<itemInfo>> items;

		struct itemDrawRange {
			int modelIndex;
			int itemBegin;
			int itemEnd; //one past end, ie use <
		};
		vector<vector<itemDrawRange>> sequences;

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


		void sequence();
		void renderFunc(int threadIndex);
		void renderPartial(int threadIndex);
		void render(int frameIndex, VkSemaphore signal, camera const& cam);
		bool renderDone();

		void postProcess(int frameIndex, int windowSwapIndex, vector<VkSemaphore> waits, VkSemaphore signal);

		void cleanup();

		system(instance & vkinst, string vs, string fs,
			vector<string> textures,
			vector<string> models,
			vector<string> animations);
		~system();
	};
}