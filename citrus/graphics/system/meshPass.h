#pragma once

#include <shared_mutex>
#include <atomic>

#include "citrus/graphics/system/renderSystem.h"
#include "citrus/graphics/system/sysNode.h"
#include "citrus/graphics/image.h"
#include "citrus/graphics/mesh/mesh.h"
#include "citrus/graphics/camera.h"

namespace citrus::graphics {
    class meshPass : public sysNode {
		#pragma region(config stuff)
	public:
		bool					wireframe = false;
		bool					cullBack = true;
		bool					ccw = true;
		bool					cullObscured = true;
		bool					texturesEnabled = true;
		bool					animated = false;
		#pragma endregion

		#pragma region(pipeline stuff)
    protected:
		fpath vert, frag;

		VkDescriptorSetLayout	uboLayout, texLayout;
		VkDescriptorPool		uboPool, texPool;
		VkDescriptorSet			uboSets[SWAP_FRAMES];
		VkDescriptorSet			texSet;

		VkRenderPass			pass;
		VkPipelineLayout		pipelineLayout;
		VkPipeline				pipeline;
		VkCommandBuffer			priBufs[SWAP_FRAMES];
		vector<VkCommandBuffer>	secBufs[SWAP_FRAMES];
		VkSemaphore				startSem[SWAP_FRAMES];
		VkSemaphore				doneSem[SWAP_FRAMES];
		VkFence					waitFences[SWAP_FRAMES];

		struct frame {
			VkImage			color;					// color image
			VkImage			depth;					// depth image
			VkImageView		colorView;				// color view
			VkImageView		depthView;				// depth view
			VkSampler		colorSamp;				// color sampler
			VkSampler		depthSamp;				// color sampler
			VkDeviceMemory	colorMem;				// color memory
			VkDeviceMemory	depthMem;				// depth memory
			VkFramebuffer	fbo;					// framebuffer
		};
		frame				frames[SWAP_FRAMES];	// frame info

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
			mat4				mvp;
			mat4x3				model;
			uvec4				uints;
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

		// get image info for color buffer
		VkDescriptorImageInfo	getColorInfo();
		// get image info for depth buffer
		VkDescriptorImageInfo	getDepthInfo();

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
        virtual void            postRender(uint32_t const& threadCount);
		
		meshPass(system & sys, fpath const& vert, fpath const& frag);
		~meshPass();
	};
}