#include "citrus/graphics/model.h"
#include "citrus/graphics/mesh.h"

namespace citrus::graphics {
	class instance;
	class mesh;
	
	void model::bindVertexAndIndexBuffers(VkCommandBuffer commands) const {
		VkBuffer vertexBuffers[8];
		for (int i = 0; i < numOffsets; i++) {
			vertexBuffers[i] = inst.vertexMem.buf;
		}
		vkCmdBindVertexBuffers(commands, 0, numOffsets, vertexBuffers, offsets);	

		vkCmdBindIndexBuffer(commands, inst.indexMem.buf, indexMem, VK_INDEX_TYPE_UINT16);
	}
	void model::cmdDraw(VkCommandBuffer commands) const {
		vkCmdDrawIndexed(commands, count, 1, 0, 0, 0);
	}
	
	model::model(instance& inst, mesh& me) : inst(inst) {
		uint64_t memSize = me.requiredMemory();
		uint64_t indexMemSize = me.index.size() * sizeof(uint16_t);

        vertexMem = inst.vertexMem.alloc(memSize);
		indexMem = inst.indexMem.alloc(indexMemSize);
        
		count = me.index.size();

		inst.fillBuffer(inst.vertexMem.buf, memSize, vertexMem, [&me](void* data){ me.constructContinuous(data); });
		inst.fillBuffer(inst.indexMem.buf, indexMemSize, indexMem, me.index.data());
		
		vector<uint64_t> meshOffsets = me.offsets();
		numOffsets = meshOffsets.size();
		for(int i = 0; i < numOffsets; i++) {
			offsets[i] = vertexMem + meshOffsets[i];
		}
	}
	model::~model() {
		inst.vertexMem.free(vertexMem);
		inst.indexMem.free(indexMem);
	}
}
