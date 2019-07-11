#include "citrus/graphics/system/renderSystem.h"
#include <fstream>
#include <shared_mutex>

namespace citrus::graphics {
	void system::createFramebufferData() {
		VkImageCreateInfo colorInfo = {};
		colorInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colorInfo.imageType = VK_IMAGE_TYPE_2D;
		colorInfo.extent.width = inst.width;
		colorInfo.extent.height = inst.height;
		colorInfo.extent.depth = 1;
		colorInfo.mipLevels = 1;
		colorInfo.arrayLayers = 1;
		colorInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		colorInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		colorInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		colorInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		colorInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkImageCreateInfo depthInfo = {};
		depthInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthInfo.imageType = VK_IMAGE_TYPE_2D;
		depthInfo.extent.width = inst.width;
		depthInfo.extent.height = inst.height;
		depthInfo.extent.depth = 1;
		depthInfo.mipLevels = 1;
		depthInfo.arrayLayers = 1;
		depthInfo.format = inst.findDepthFormat();
		depthInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		depthInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		depthInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateImage(inst._device, &colorInfo, nullptr, &frames[i].color) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			if (vkCreateImage(inst._device, &depthInfo, nullptr, &frames[i].depth) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image!");
			}

			VkMemoryRequirements colorRequirements, depthRequirements;
			vkGetImageMemoryRequirements(inst._device, frames[i].color, &colorRequirements);
			vkGetImageMemoryRequirements(inst._device, frames[i].depth, &depthRequirements);

			VkMemoryAllocateInfo colorAllocInfo = {}, depthAllocInfo = {};
			colorAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			colorAllocInfo.memoryTypeIndex = inst.findMemoryType(colorRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			colorAllocInfo.allocationSize = colorRequirements.size;
			depthAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			depthAllocInfo.memoryTypeIndex = inst.findMemoryType(depthRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			depthAllocInfo.allocationSize = depthRequirements.size;
			vkAllocateMemory(inst._device, &colorAllocInfo, nullptr, &frames[i].colorMem);
			vkAllocateMemory(inst._device, &depthAllocInfo, nullptr, &frames[i].depthMem);

			vkBindImageMemory(inst._device, frames[i].color, frames[i].colorMem, 0);
			vkBindImageMemory(inst._device, frames[i].depth, frames[i].depthMem, 0);

			VkImageViewCreateInfo colorViewInfo = {}, depthViewInfo = {};
			colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			colorViewInfo.image = frames[i].color;
			colorViewInfo.subresourceRange.baseMipLevel = 0;
			colorViewInfo.subresourceRange.baseArrayLayer = 0;
			colorViewInfo.subresourceRange.levelCount = 1;
			colorViewInfo.subresourceRange.layerCount = 1;
			colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthViewInfo.format = inst.findDepthFormat();
			depthViewInfo.image = frames[i].depth;
			depthViewInfo.subresourceRange.baseMipLevel = 0;
			depthViewInfo.subresourceRange.baseArrayLayer = 0;
			depthViewInfo.subresourceRange.levelCount = 1;
			depthViewInfo.subresourceRange.layerCount = 1;
			depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			vkCreateImageView(inst._device, &colorViewInfo, nullptr, &frames[i].colorView);
			vkCreateImageView(inst._device, &depthViewInfo, nullptr, &frames[i].depthView);

			VkSamplerCreateInfo sampInfo = {};
			sampInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampInfo.magFilter = VK_FILTER_NEAREST;
			sampInfo.minFilter = VK_FILTER_NEAREST;
			sampInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampInfo.anisotropyEnable = VK_FALSE;
			sampInfo.maxAnisotropy = 1;
			sampInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampInfo.unnormalizedCoordinates = VK_FALSE;
			sampInfo.compareEnable = VK_FALSE;
			sampInfo.compareOp = VK_COMPARE_OP_NEVER;
			sampInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			sampInfo.mipLodBias = 0.0f;
			sampInfo.minLod = 0.0f;
			sampInfo.maxLod = 0.0f;

			vkCreateSampler(inst._device, &sampInfo, nullptr, &frames[i].colorSamp);
			frames[i].depthSamp = VK_NULL_HANDLE;
		}
	}
	void system::freeFramebufferData() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			//vkDestroyFramebuffer(inst._device, frames[i].frameBuffer, nullptr);
			//vkDestroySampler(inst._device, frames[i].depthSamp, nullptr);
			vkDestroySampler(inst._device, frames[i].colorSamp, nullptr);
			vkDestroyImageView(inst._device, frames[i].colorView, nullptr);
			vkDestroyImageView(inst._device, frames[i].depthView, nullptr);
			vkDestroyImage(inst._device, frames[i].color, nullptr);
			vkDestroyImage(inst._device, frames[i].depth, nullptr);
			vkFreeMemory(inst._device, frames[i].colorMem, nullptr);
			vkFreeMemory(inst._device, frames[i].depthMem, nullptr);
		}
	}
	void system::loadTextures() {
		if (texturePaths.size() == 0) throw std::runtime_error("there must be at least one texture to load");
		uint64_t addr = 0;
		uint32_t typeBits = 0;
		for (int i = 0; i < texturePaths.size(); i++) {
			textures.push_back({});

			textures[i].data = std::move(image4b(texturePaths[i].string()));

			textures[i].format = VK_FORMAT_R8G8B8A8_UNORM;

			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			info.extent.width = textures[i].data.width();
			info.extent.height = textures[i].data.height();
			info.extent.depth = 1;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.format = VK_FORMAT_R8G8B8A8_UNORM;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.samples = VK_SAMPLE_COUNT_1_BIT;

			vkCreateImage(inst._device, &info, nullptr, &textures[i].img);

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(inst._device, textures[i].img, &memRequirements);
			if (typeBits == 0) typeBits = memRequirements.memoryTypeBits;
			else if (typeBits != memRequirements.memoryTypeBits) throw std::runtime_error("different image memory requirements :(");

			addr = util::roundUpAlign(addr, memRequirements.alignment);
			textures[i].off = addr;
			addr += memRequirements.size;
		}

		VkMemoryAllocateInfo memInfo = {};
		memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memInfo.allocationSize = addr;
		memInfo.memoryTypeIndex = inst.findMemoryType(typeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkAllocateMemory(inst._device, &memInfo, nullptr, &textureMem);

		for (int i = 0; i < texturePaths.size(); i++) {
			vkBindImageMemory(inst._device, textures[i].img, textureMem, textures[i].off);

			inst.pipelineBarrierLayoutChange(textures[i].img,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0, VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
			uint64_t stagingAddr = inst.stagingMem.alloc(textures[i].data.size());
			inst.mapUnmapMemory(inst.stagingMem.mem, textures[i].data.size(), stagingAddr, textures[i].data.data());
			inst.copyBufferToImage(inst.stagingMem.buf, stagingAddr, textures[i].img, textures[i].data.width(), textures[i].data.height());
			inst.stagingMem.free(stagingAddr);
			inst.pipelineBarrierLayoutChange(textures[i].img, VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			viewInfo.image = textures[i].img;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			vkCreateImageView(inst._device, &viewInfo, nullptr, &textures[i].view);

			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_NEAREST;
			samplerInfo.minFilter = VK_FILTER_NEAREST;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.maxAnisotropy = 1;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;

			vkCreateSampler(inst._device, &samplerInfo, nullptr, &textures[i].samp);
		}
	}
	void system::freeTextures() {
		vkFreeMemory(inst._device, textureMem, nullptr);
		for (int t = 0; t < textures.size(); t++) {
			vkDestroySampler(inst._device, textures[t].samp, nullptr);
			vkDestroyImageView(inst._device, textures[t].view, nullptr);
			vkDestroyImage(inst._device, textures[t].img, nullptr);
		}
	}

	void system::collectModelInfo() {
		VkBufferCreateInfo vertexInfo = {};
		vertexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vertexInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertexInfo.size = 1024 * 1024; // buffer never actually uses memory

		VkBufferCreateInfo indexInfo = {};
		indexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		indexInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexInfo.size = 1024 * 1024; // buffer never actually uses memory

		VkBuffer tmpVertexBuf, tmpIndexBuf;
		vkCreateBuffer(inst._device, &vertexInfo, nullptr, &tmpVertexBuf);
		vkCreateBuffer(inst._device, &indexInfo, nullptr, &tmpIndexBuf);

		vkGetBufferMemoryRequirements(inst._device, tmpVertexBuf, &vertexRequirements);
		vkGetBufferMemoryRequirements(inst._device, tmpIndexBuf, &indexRequirements);

		vkDestroyBuffer(inst._device, tmpVertexBuf, nullptr);
		vkDestroyBuffer(inst._device, tmpIndexBuf, nullptr);
	}
	void system::loadModels() {
		uint64_t vertexAddr = 0, indexAddr = 0;

		models.resize(modelPaths.size());

		for (int i = 0; i < modelPaths.size(); i++) {
			models[i].m = mesh(modelPaths[i].string());
			models[i].desc = models[i].m.getDescription(vertexAddr, vertexRequirements.alignment, indexAddr, indexRequirements.alignment);
			models[i].radius = models[i].m.getMaxRadius();
		}
	}
	void system::initializeModelData() {
		VkDeviceAddress totalVertexSize = models.back().desc.nextFree;
		VkDeviceAddress totalIndexSize = models.back().desc.nextFreeIndex;

		VkBufferCreateInfo vertexInfo = { };
		vertexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vertexInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		vertexInfo.size = totalVertexSize;

		vkCreateBuffer(inst._device, &vertexInfo, nullptr, &vertexBuffer);
		for (int i = 0; i < (sizeof(vertexBuffers) / sizeof(vertexBuffers[0])); i++) vertexBuffers[i] = vertexBuffer;

		VkMemoryRequirements vertexRequirements;
		vkGetBufferMemoryRequirements(inst._device, vertexBuffer, &vertexRequirements);

		VkMemoryAllocateInfo vertexMemInfo = { };
		vertexMemInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vertexMemInfo.allocationSize = vertexRequirements.size;
		vertexMemInfo.memoryTypeIndex = inst.findMemoryType(vertexRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(inst._device, &vertexMemInfo, nullptr, &vertexMemory);

		VkBufferCreateInfo indexInfo = { };
		indexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		indexInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		indexInfo.size = totalIndexSize;

		vkCreateBuffer(inst._device, &indexInfo, nullptr, &indexBuffer);

		VkMemoryRequirements indexRequirements;
		vkGetBufferMemoryRequirements(inst._device, indexBuffer, &indexRequirements);

		VkMemoryAllocateInfo indexMemInfo = { };
		indexMemInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		indexMemInfo.allocationSize = indexRequirements.size;
		indexMemInfo.memoryTypeIndex = inst.findMemoryType(indexRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(inst._device, &indexMemInfo, nullptr, &indexMemory);

		vkBindBufferMemory(inst._device, vertexBuffer, vertexMemory, 0);
		vkBindBufferMemory(inst._device, indexBuffer, indexMemory, 0);

		uint64_t vertexStaging = inst.stagingMem.alloc(totalVertexSize);
		void* vertexScratch = malloc(totalVertexSize);
		for (int i = 0; i < models.size(); i++) {
			models[i].m.fillVertexData(vertexScratch, models[i].desc);
		}
		inst.mapUnmapMemory(inst.stagingMem.mem, totalVertexSize, vertexStaging, vertexScratch);
		inst.copyBuffer(inst.stagingMem.buf, vertexBuffer, totalVertexSize, vertexStaging, 0);
		inst.stagingMem.free(vertexStaging);
		free(vertexScratch);

		uint64_t indexStaging = inst.stagingMem.alloc(totalIndexSize);
		void* indexScratch = malloc(totalIndexSize);
		for (int i = 0; i < models.size(); i++) {
			models[i].m.fillIndexData(indexScratch, models[i].desc);
		}
		inst.mapUnmapMemory(inst.stagingMem.mem, totalIndexSize, indexStaging, indexScratch);
		inst.copyBuffer(inst.stagingMem.buf, indexBuffer, totalIndexSize, indexStaging, 0);
		inst.stagingMem.free(indexStaging);
		free(indexScratch);
	}
	void system::freeModels() {
		vkFreeMemory(inst._device, vertexMemory, nullptr);
		vkFreeMemory(inst._device, indexMemory, nullptr);
		vkDestroyBuffer(inst._device, vertexBuffer, nullptr);
		vkDestroyBuffer(inst._device, indexBuffer, nullptr);
	}
	void system::loadAnimations() {
		for (int i = 0; i < animationPaths.size(); i++) {
			std::ifstream f(animationPaths[i]);
			animation ani;
			ani.read(f);
			animations.push_back(ani);
		}
		for (int i = 0; i < animations.size(); i++) {
			for (int j = 0; j < models.size(); j++) {
				bool success = models[j].m.bindAnimation(animations[i]);
				if (success) util::sout("animation bound: model " + std::to_string(j) + " lod 0, ani " + std::to_string(i) + "\n");
			}
		}
	}

	void system::initializeThreads(uint32_t threadCount) {
		if (threadCount < 1) threadCount = 1;
		else if (threadCount > 256) threadCount = 256;

		renderThreads.resize(threadCount);
		for (size_t i = 0; i < SWAP_FRAMES; i++) {
			primaryBuffers[i] = VK_NULL_HANDLE;
		}
		commandPools.resize(renderThreads.size());
		for (size_t t = 0; t < renderThreads.size(); t++) {
			renderGo[t] = false;
			commandPools[t] = inst.createCommandPool();
		}

		for (size_t t = 1; t < renderThreads.size(); t++) {
			renderThreads[t] = std::thread(&system::renderFunc, this, t);
		}
	}
	void system::freeThreads() {
		for (int t = 1; t < renderThreads.size(); t++) {
			renderThreads[t].join();
		}
		stdioDebug("all threads joined\n");

		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (primaryBuffers[i] != VK_NULL_HANDLE) vkFreeCommandBuffers(inst._device, inst._commandPool, 1, &primaryBuffers[i]);
		}
		for (int t = 0; t < renderThreads.size(); t++) {
			vkDestroyCommandPool(inst._device, commandPools[t], nullptr);
		}
	}

	void system::initializeUniformData() {
		uniformAlignment = inst.minUniformBufferOffsetAlignment();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = uniformSize;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		for (int i = 0; i < SWAP_FRAMES; i++) {
			if (vkCreateBuffer(inst._device, &bufferInfo, nullptr, &uniformBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create memory buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(inst._device, uniformBuffers[i], &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = inst.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			if (vkAllocateMemory(inst._device, &allocInfo, nullptr, &uniformMemories[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate buffer memory!");
			}

			vkBindBufferMemory(inst._device, uniformBuffers[i], uniformMemories[i], 0);

			vkMapMemory(inst._device, uniformMemories[i], 0, uniformSize, 0, (void**)&uniformMapped[i]);
		}
	}
	void system::freeUniformData() {
		for (int i = 0; i < SWAP_FRAMES; i++) {
			vkUnmapMemory(inst._device, uniformMemories[i]);
			vkDestroyBuffer(inst._device, uniformBuffers[i], nullptr);
			vkFreeMemory(inst._device, uniformMemories[i], nullptr);
		}
	}

	/*void system::sequence(vector<vector<itemInfo>> const& items, vector<vector<itemDrawRange>> & ranges) {
		int totalItems = 0;
		for (int m = 0; m < items.size(); m++)
			totalItems += items[m].size();

		hptime seqStart = hpclock::now();
		for (int t = 0; t < renderThreads.size(); t++)
			for (int m = 0; m < items.size(); m++)
				ranges[t][m].modelIndex = -1;

		int target = totalItems / renderThreads.size() + 1;
		int modelIndex = 0, itemBegin = 0, threadIndex = 0, threadRangeIndex = 0, itemsInThread = 0;
		while (true) {
			if (threadIndex != (renderThreads.size() - 1) && itemsInThread == target) {
				itemsInThread = 0;
				threadIndex++;
				threadRangeIndex = 0;
			}

			if (itemBegin == items[modelIndex].size()) {
				modelIndex++;
				itemBegin = 0;
				if (modelIndex == items.size()) break;
			}

			int toAdd = target;
			if (itemsInThread + toAdd > target) {
				toAdd = target - itemsInThread;
			}
			if (itemBegin + toAdd > items[modelIndex].size()) {
				toAdd = items[modelIndex].size() - itemBegin;
			}

			ranges[threadIndex][threadRangeIndex] = { modelIndex, itemBegin, itemBegin + toAdd };
			threadRangeIndex++;

			itemBegin += toAdd;
			itemsInThread += toAdd;
		}
		stdioDebug("sequencing took " + std::to_string((hpclock::now() - seqStart).count() * 0.000001) + " ms\n");
	}*/
	void system::renderFunc(uint32_t threadIndex) {
		if (threadIndex == 0) throw std::runtime_error("started thread 0 (should happen on main thread)");
		while (!stopped) {
			if (renderGo[threadIndex]) {
				passes[currentPass]->renderPartial(threadIndex);
				renderGo[threadIndex] = false;
			} else {
				//std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
		}
		renderGo[threadIndex] = false;
		stdioDebug("completed thread " + std::to_string(threadIndex) + "\n");
	}
	bool system::renderDone() const {
		for (int i = 0; i < renderThreads.size(); i++) {
			if (renderGo[i] == true) return false;
		}
		return true;
	}
	void system::postProcess(int frameIndex, int windowSwapIndex, vector<VkSemaphore> waits, VkSemaphore signal) {
		/*VkDescriptorImageInfo colorInfo = {};
		colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorInfo.imageView = frames[frameIndex].colorView;
		colorInfo.sampler = frames[frameIndex].samp;

		VkDescriptorImageInfo depthInfo = {};
		depthInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		depthInfo.imageView = frames[frameIndex].depthView;
		depthInfo.sampler = frames[frameIndex].samp;

		inst._finalPass->fillCommandBuffer(windowSwapIndex, colorInfo, depthInfo);
		inst._finalPass->submit(windowSwapIndex, waits, signal);*/
	}
	void system::render(VkSemaphore sem, camera const& cam) {
		{ std::unique_lock<std::shared_mutex> lock(startMut);
			frameCam = cam;
			frameCull = cam.genFrustrumInfo();
			frameVP = cam.getViewProjectionMatrix();
		}

		for(uint32_t i = 0; i < passes.size(); i++) {
			currentPass = i;

			passes[i]->preRender(renderThreads.size());

			for(int j = 1; j < renderThreads.size(); j++) {
				renderGo[j] = true;
			}

			passes[i]->renderPartial(0);

			renderGo[0] = false;

			while (!renderDone()) {
				//std::this_thread::sleep_for(std::chrono::microseconds(100));
			}

			passes[i]->postRender((uint32_t) renderThreads.size());
		}
	}
	system::system(instance& vkinst, fpath texturePath, fpath modelPath, fpath animationPath) : inst(vkinst) {
		texturePaths = util::filesInDirectory(texturePath, ".png");
		modelPaths = util::filesInDirectory(modelPath, ".dae");
		animationPaths = util::filesInDirectory(animationPath, ".cta");
		uniformSize = 1024 * 1024 * 16;

		createFramebufferData();

		loadTextures();

		collectModelInfo();
		loadModels();
		initializeModelData();
		loadAnimations();

		stopped = false;
	}
	system::~system() {
		stopped = true;

		freeTextures();

		freeModels();

		freeFramebufferData();

		freeThreads();
	}
}