#include <citrus/graphics/model.h>
#include <citrus/graphics/mesh.h>

namespace citrus::graphics {
	class instance;
	class mesh;
	
	void model::cmdDraw(VkCommandBuffer commands) const {
		VkBuffer vertexBuffers[8];
		for(int i = 0; i < numOffsets; i++) {
			vertexBuffers[i] = inst.vertexMem.buf;
		}
		vkCmdBindVertexBuffers(commands, 0, numOffsets, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commands, inst.indexMem.buf, indexMem, VK_INDEX_TYPE_UINT16);
		
		vkCmdDrawIndexed(commands, count, 1, 0, 0, 0);
	}
	
	model::model(instance& inst, mesh& me) : inst(inst) {
		uint64_t memSize = me.requiredMemory();
		uint64_t indexMemSize = me.index.size() * sizeof(uint16_t);
		
		uint64_t stagingMem = inst.stagingMem.alloc(memSize);
		uint64_t indexStagingMem = inst.indexMem.alloc(indexMemSize);
		
		vertexMem = inst.vertexMem.alloc(memSize);
		indexMem = inst.indexMem.alloc(indexMemSize);
		
		inst.fillBuffer(inst.stagingMem.buf, memSize, stagingMem, std::function<void(void*)>([&me](void* data){ me.constructContinuous(data); }));
		inst.fillBuffer(inst.stagingMem.buf, memSize, indexStagingMem, me.index.data());
		
		inst.copyBuffer(inst.stagingMem.buf, inst.vertexMem.buf, memSize, stagingMem, vertexMem);
		inst.copyBuffer(inst.stagingMem.buf, inst.indexMem.buf, indexMemSize, indexStagingMem, indexMem);
		
		inst.stagingMem.free(stagingMem);
		inst.stagingMem.free(indexStagingMem);
		
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