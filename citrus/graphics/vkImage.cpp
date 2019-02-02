#include <citrus/graphics/vkImage.h>
#include <citrus/graphics/image.h>
#include <citrus/graphics/instance.h>

namespace citrus::graphics {
	template<typename P>
	void vkImageT::transferLayout(VkImageLayout pre, VkImageLayout post, fenceProc *proc) {
		
	}
	
	template<typename P>
	void vkImageT::set(const imageT<P>& img, fenceProc *proc) {
		
	}
	
	template<typename P>
	vkImageT::vkImageT(instance& inst, const imageT<P>& img) {
		inst.createImage(img.width(), img.height(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _image, _memory);
		_inst.pipelineBarrierLayoutChange(_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VkBuffer buf;
		_inst.createBuffer(
		_inst.copyBufferToImage(_
	}
	
	template class vkImageT<pixel1<unsigned char>>;
	template class vkImageT<pixel2<unsigned char>>;
	template class vkImageT<pixel3<unsigned char>>;
	template class vkImageT<pixel4<unsigned char>>;
	
	template class vkImageT<pixel1<float>>;
	template class vkImageT<pixel2<float>>;
	template class vkImageT<pixel3<float>>;
	template class vkImageT<pixel4<float>>;
	
	template class vkImageT<pixel1<int>>;
	template class vkImageT<pixel2<int>>;
	template class vkImageT<pixel3<int>>;
	template class vkImageT<pixel4<int>>;
	
	template class vkImageT<pixel1<unsigned int>>;
	template class vkImageT<pixel2<unsigned int>>;
	template class vkImageT<pixel3<unsigned int>>;
	template class vkImageT<pixel4<unsigned int>>;
	
	using vkImage1b =  vkImageT<pixel1<unsigned char>>;
	using vkImage2b =  vkImageT<pixel2<unsigned char>>;
	using vkImage3b =  vkImageT<pixel3<unsigned char>>;
	using vkImage4b =  vkImageT<pixel4<unsigned char>>;
	
	using vkImage1f =  vkImageT<pixel1<float>>;
	using vkImage2f =  vkImageT<pixel2<float>>;
	using vkImage3f =  vkImageT<pixel3<float>>;
	using vkImage4f =  vkImageT<pixel4<float>>;
	
	using vkImage1i =  vkImageT<pixel1<int>>;
	using vkImage2i =  vkImageT<pixel2<int>>;
	using vkImage3i =  vkImageT<pixel3<int>>;
	using vkImage4i =  vkImageT<pixel4<int>>;
	
	using vkImage1ui = vkImageT<pixel1<unsigned int>>;
	using vkImage2ui = vkImageT<pixel2<unsigned int>>;
	using vkImage3ui = vkImageT<pixel3<unsigned int>>;
	using vkImage4ui = vkImageT<pixel4<unsigned int>>;

	using vkImage = vkImage3b; //default rgb 0-255 image
	using vkImagea = vkImage4b; //default rgba 0-255 image
}