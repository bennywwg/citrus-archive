#pragma once

#include <shared_mutex>
#include <atomic>

#include "citrus/graphics/system/renderSystem.h"
#include "citrus/graphics/system/sysNode.h"
#include "citrus/graphics/image.h"
#include "citrus/graphics/mesh/mesh.h"
#include "citrus/graphics/camera.h"
#include "citrus/graphics/system/runtimeResource.h"

namespace citrus::graphics {
    class meshPass : public sysNode {
		#pragma region(config stuff)
	public:
		bool					wireframe = false;
		bool					cullBack = true;
		bool					ccw = true;
		bool					cullObscured = true;
		bool					texturesEnabled = true;
		bool					rigged = false;
		uint64_t const			uboSize = 4 * 1024;
		uint64_t const			ssboSize = 4 * 1024 * 4;
		#pragma endregion

		#pragma region(pipeline stuff)
    protected:
		fpath vert, frag;

		bool					transitionToRead;

		VkDescriptorSetLayout	uboLayout, ssboLayout, texLayout, cubeLayout;
		VkDescriptorPool		uboPool, ssboPool, texPool, cubePool;
		VkDescriptorSet			uboSets[SWAP_FRAMES];
		VkDescriptorSet			ssboSets[SWAP_FRAMES];
		VkDescriptorSet			texSet, cubeSet;

		VkRenderPass			pass;
		VkPipelineLayout		pipelineLayout;
		VkPipeline				pipeline;
		VkCommandBuffer			priBufs[SWAP_FRAMES];
		vector<VkCommandBuffer>	secBufs[SWAP_FRAMES];
		VkFence					waitFences[SWAP_FRAMES];
		
		struct uniformBlock {
			vec4 camPos;
			vec4 lightDir;
		};
		buffer					ubos[SWAP_FRAMES];
		buffer					ssbos[SWAP_FRAMES];
		buffer					stagings[SWAP_FRAMES];

		VkCommandBuffer			stagingCommands[SWAP_FRAMES];
		VkSemaphore				stagingSems[SWAP_FRAMES];

	public:
		frameStore* const	frame;
	protected:
		VkFramebuffer		fbos[SWAP_FRAMES];

		VkCommandBufferInheritanceInfo inheritanceInfos[SWAP_FRAMES];

		meshUsageLocationMapping meshMappings;
		
		virtual void			initializeDescriptors();
		virtual void			initializeRenderPass();
		virtual void			initializePipelineLayout();
		virtual void			initializePipeline();
		virtual void			initializeFramebuffers();
    public:
		#pragma endregion

		#pragma region(item stuff)
    protected:
		struct pcData {
			mat4	mvp;
			float	rowMajorModel[4*3];
			uvec4	uints;
		};
		const uint32_t pcVertSize = 128 - 16;
		const uint32_t pcFragSize = 16;
		static_assert(sizeof(pcData) == 128, "pcData must be 128 bytes");
    public:

		struct itemInfo {
			vec3				pos;
			quat				ori;
			uint32_t			modelIndex;
			uint32_t			texIndex;
			uint32_t			normalTexIndex;
			uint32_t			animationIndex;
			float				aniTime;
			uint32_t			uniformOffset;
			uint32_t			uniformSize;
			bool				enabled;
		};
		vector<itemInfo>		items;

    protected:
		struct threadData {
			uint32_t			offset;		//offset into uniform buffer for this thread
			uint32_t			size;		//size of thread's data
			uint32_t			begin;		// first item
			uint32_t			end;		// one past end, ie use <
		};
		vector<threadData>		ranges;
    public:
		#pragma endregion

		#pragma region(model stuff)
    protected:
		//mappings to models in system
		struct modelMapping {
			int modelIndex;
			meshMemoryStructure desc;
		};
		vector<modelMapping>	mappings;
		vector<meshAttributeUsage> requiredUsages;
		meshAttributeUsage		allUsages;

		virtual void			mapModels();
    public:
		#pragma endregion

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
        virtual void            postRender(uint32_t const& threadCount);
		
		meshPass(system & sys, frameStore* fstore, bool textured, bool lit, bool rigged, fpath const& vert, fpath const& frag, bool transitionToRead);
		~meshPass();
	};
}