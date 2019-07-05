#pragma once

#include <shared_mutex>
#include <atomic>

#include "citrus/graphics/system/instance.h"
#include "citrus/graphics/image.h"
#include "citrus/graphics/mesh/mesh.h"
#include "citrus/graphics/camera.h"

namespace citrus::graphics {
    class meshPass : public passBase {
		#pragma region(pipeline stuff)
    protected:
		string vert, frag;

		VkDescriptorSetLayout	uboLayout;
		VkDescriptorSetLayout	texLayout;
		VkDescriptorPool		uboPool;
		VkDescriptorSet			uboSets;

		VkRenderPass			pass;
		VkPipelineLayout		pipelineLayout;
		VkPipeline				pipeline;
		VkFramebuffer			fbos[SWAP_FRAMES];

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

		virtual void			mapModels();
    public:
		#pragma endregion

		virtual void			preRender(uint32_t const& threadCount);
		virtual void 			renderPartial(uint32_t const& threadIndex);
        virtual void            postRender(uint32_t const& threadCount);
		
		meshPass(system & sys, string const& vert, string const& frag);
		~meshPass();
	};
}