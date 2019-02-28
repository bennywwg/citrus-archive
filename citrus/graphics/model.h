#include <citrus/util.h>
#include <citrus/graphics/instance.h>

namespace citrus::graphics {
	class instance;
	class mesh;
	
	class model {
	public:
		instance& inst;
	
		uint64_t vertexMem, indexMem;
		uint32_t count;
		
		uint32_t numOffsets;
		uint64_t offsets[8]; //not all slots are used
		
		void cmdDraw(VkCommandBuffer commands) const;
	
		model(instance& inst, mesh& me);
		~model();
		
		NO_COPY(model)
	};
}