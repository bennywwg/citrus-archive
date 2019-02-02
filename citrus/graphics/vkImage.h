#pragma once

#includes <citrus/graphics/image.h>

namespace citrus::graphics {
	class instance;
	class fenceProc;
	class vkTexture;
	
	template<typename P>
	class vkImageT {
		friend class vkTexture;
		
		instance& _inst;
		VkImage _image;
		VkDeviceMemory _memory;
		
	public:
		void transferLayout(VkImageLayout pre, VkImageLayout post, fenceProc *proc = nullptr);

		void set(const imageT<P>& img, fenceProc *proc = nullptr);
		
		vkImageT(instance& inst, const imageT<P>& img);
	};
}