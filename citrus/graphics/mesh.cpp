#include "citrus/graphics/mesh.h"

#include "citrus/graphics/instance.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>

namespace citrus::graphics {
	static inline glm::mat4 convertMat(const aiMatrix4x4& m) {
		/*return glm::mat4(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4);*/
		return glm::mat4(
			m.a1, m.b1, m.c1, m.d1,
			m.a2, m.b2, m.c2, m.d2,
			m.a3, m.b3, m.c3, m.d3,
			m.a4, m.b4, m.c4, m.d4);
	}

	int nodeContainer::addNodes(const aiNode* n, int parent) {
		int ci = nodes.size();
		nodes.emplace_back();
		nodes[ci].name = n->mName.C_Str();
		nodes[ci].parent = parent;
		nodes[ci].transform = convertMat(n->mTransformation);
		nodes[ci].children.resize(n->mNumChildren);
		for(int i = 0; i < n->mNumChildren; i++) {
			int childIndex = addNodes(n->mChildren[i], ci);
			nodes[ci].children[i] = childIndex;
		}
		return ci;
	}
	void nodeContainer::loadAllNodes(const aiScene* scene) {
		addNodes(scene->mRootNode);
		for(int i = 0; i < nodes.size(); i++) {
			nameMap[nodes[i].name] = i;
		}
	}

	void vertexBoneInfo::normalize()  {
		if(primary != -1) {
			if(secondary == -1) {
				primaryWeight = 1.0f;
			} else {
				float total = primaryWeight + secondaryWeight;
				primaryWeight /= total;
				secondaryWeight /= total;
			}
		}
	}

	vertexBoneInfo boneContainer::getInfo(unsigned int vertex) {
		vertexBoneInfo res;
		for(int i = 0; i < bones.size(); i++) {
			float val = weights[i][vertex];
			if(val <= 0.0f) continue;

			if(res.primary == -1) {
				res.primary = i;
				res.primaryWeight = val;
			} else if(val > res.primaryWeight && res.secondary == -1) {
				res.secondary = res.primary;
				res.secondaryWeight = res.primaryWeight;
				res.primary = i;
				res.primaryWeight = val;
			} else if(res.secondary == -1 || val > res.secondaryWeight) {
				res.secondary = i;
				res.secondaryWeight = val;
			}
		}
		return res;
	}
	void boneContainer::loadAllBones(const aiScene* scene, const nodeContainer& nodes) {
		const aiMesh* mesh = scene->mMeshes[0];

		for(size_t j = 0; j < mesh->mNumBones; j++) {
			const aiBone& inBone = *mesh->mBones[j];

			string name = inBone.mName.C_Str();
			auto nodeIndexIt = nodes.nameMap.find(name);
			if(nodeIndexIt == nodes.nameMap.end())
				throw std::runtime_error("Mesh error bone has no corresponding node");

			bones.emplace_back();
			bone& bone = bones.back();
			bone.node = nodeIndexIt->second;
			bone.offset = convertMat(inBone.mOffsetMatrix);
			weights.emplace_back(scene->mMeshes[0]->mNumVertices, -1.0f);
			for (size_t k = 0; k < inBone.mNumWeights; k++) {
				weights[j][inBone.mWeights[k].mVertexId] = inBone.mWeights[k].mWeight;
			}

			nameMap[name] = j;
		}
	}

	template<typename T>
	T animation::key<T>::mix(const double& time, const key<T>& l, const key<T>& r) {
		return glm::mix(l.val, r.val, (float)((time - l.time) / (r.time - l.time)));
	}
	template<typename T>
	animation::key<T>::key(const double& time, const T& val) : time(time), val(val) { }
	template struct animation::key<vec3>;
	template struct animation::key<quat>;

	template<typename T>
	T animation::channel::get(double time, std::vector<key<T>>const& keys) const {
		if(keys.size() == 1) {
			return keys.front().val;
		} else {
			if(time < keys.front().time) {
				return keys.front().val;
			} else if(time >= keys.back().time) {
				return keys.back().val;
			} else {
				for(int i = 0; i < keys.size() - 1; i++) {
					if(time >= keys[i].time && time < keys[i + 1].time) {
						return key<T>::mix(time, keys[i], keys[i + 1]);
					}
				}
			}
		}
		throw std::runtime_error("Never found proper keys to interpolate from");
	}
	vec3 animation::channel::getPosition(double time) const {
		return get(time, positions);
	}
	quat animation::channel::getOrientation(double time) const {
		return get(time, orientations);
	}
	vec3 animation::channel::getScaling(double time) const {
		return get(time, scalings);
	}
	void animation::channel::write(std::ostream& s) const {
		s << "c " << nodeName << std::endl;
		s << "p " << positions.size() << std::endl;
		for(int i = 0; i < positions.size(); i++)
			s << positions[i].time << " " << positions[i].val.x << " " << positions[i].val.y << " " << positions[i].val.z << std::endl;
		s << "o " << orientations.size() << std::endl;
		for(int i = 0; i < orientations.size(); i++)
			s << orientations[i].time << " " << orientations[i].val.w << " " << orientations[i].val.x << " " << orientations[i].val.y << " " << orientations[i].val.z << std::endl;
		s << "s " << scalings.size() << std::endl;
		for(int i = 0; i < scalings.size(); i++)
			s << scalings[i].time << " " << scalings[i].val.x << " " << scalings[i].val.y << " " << scalings[i].val.z << std::endl;
	}
	void animation::channel::read(std::istream& s) {
		std::string line;
		char mode = 'n';
		int remaining = 0;
		bool pd = false, od = false, sd = false;
		while(true) {
			if(!std::getline(s, line)) throw std::runtime_error("Error parsing animation channel stream ended early");

			if(mode == 'n') {
				if(line[0] == 'c') {
					nodeName = line.substr(2);
				} else {
					if(remaining != 0) {
						throw std::runtime_error("Error parsing animation channel indicated keys exceeded actual amount");
					}
					if(line[0] == 'p') {
						mode = 'p';
						remaining = std::stoi(line.substr(2));
					} else if(line[0] == 'o') {
						mode = 'o';
						remaining = std::stoi(line.substr(2));
					} else if(line[0] == 's') {
						mode = 's';
						remaining = std::stoi(line.substr(2));
					} else {
						throw std::runtime_error("Error parsing animation channel expected p/o/s line");
					}
					if(remaining <= 0) throw std::runtime_error("Error parsing animation channel indicated keys is 0 or negative");
				}
			} else {
				if(remaining == 0) throw std::runtime_error(("Error parsing animation channel exceeded amount of keys for mode " + std::string(1, mode)).c_str());
				double time;
				if(mode == 'p') {
					float x, y, z;
					int s0 = line.find_first_of(' ', 0);
					time = std::stod(line.substr(0, s0));
					int s1 = line.find_first_of(' ', s0 + 1);
					x = std::stof(line.substr(s0 + 1));
					int s2 = line.find_first_of(' ', s1 + 1);
					y = std::stof(line.substr(s1 + 1));
					z = std::stof(line.substr(s2 + 1));
					positions.emplace_back(time, glm::vec3(x, y, z));
				} else if(mode == 'o') {
					float w, x, y, z;
					int s0 = line.find_first_of(' ', 0);
					time = std::stod(line.substr(0, s0));
					int s1 = line.find_first_of(' ', s0 + 1);
					w = std::stof(line.substr(s0 + 1));
					int s2 = line.find_first_of(' ', s1 + 1);
					x = std::stof(line.substr(s1 + 1));
					int s3 = line.find_first_of(' ', s2 + 1);
					y = std::stof(line.substr(s2 + 1));
					z = std::stof(line.substr(s3 + 1));
					orientations.emplace_back(time, glm::quat(w, x, y, z));
				} else if(mode == 's') {
					float x, y, z;
					int s0 = line.find_first_of(' ', 0);
					time = std::stod(line.substr(0, s0));
					int s1 = line.find_first_of(' ', s0 + 1);
					x = std::stof(line.substr(s0 + 1));
					int s2 = line.find_first_of(' ', s1 + 1);
					y = std::stof(line.substr(s1 + 1));
					z = std::stof(line.substr(s2 + 1));
					scalings.emplace_back(time, glm::vec3(x, y, z));
				}
				remaining--;
				if(remaining == 0) {
					if(mode == 'p') pd = true;
					if(mode == 'o') od = true;
					if(mode == 's') sd = true;
					mode = 'n';
				}
				if(pd && od && sd) {
					begin = glm::min(positions.front().time, glm::min(positions.front().time, positions.front().time));
					end = glm::max(positions.back().time, glm::max(positions.back().time, positions.back().time));
					return;
				}
			}
		}
	}
	void animationContainer::loadAllAnimations(const aiScene* scene) {
		for(int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* ani = scene->mAnimations[i];
			std::string name = ani->mName.C_Str();

			animations.emplace_back();
			animation& a = animations.back();
			a.name = name;
			//a.duration = ani->mDuration;

			for(int j = 0; j < ani->mNumChannels; j++) {
				aiNodeAnim* ch = ani->mChannels[j];

				string nodeName = ch->mNodeName.C_Str();

				a.channels.emplace_back();
				animation::channel& c = a.channels.back();

				c.nodeName = nodeName;
				
				c.positions.reserve(ch->mNumPositionKeys);
				for(int k = 0; k < ch->mNumPositionKeys; k++) {
					glm::vec3 v = {
						ch->mPositionKeys[k].mValue.x,
						ch->mPositionKeys[k].mValue.y,
						ch->mPositionKeys[k].mValue.z
					};
					c.positions.emplace_back(ch->mPositionKeys[k].mTime, v);
				}

				c.orientations.reserve(ch->mNumRotationKeys);
				for(int k = 0; k < ch->mNumRotationKeys; k++) {
					glm::quat r(
						ch->mRotationKeys[k].mValue.w,
						ch->mRotationKeys[k].mValue.x,
						ch->mRotationKeys[k].mValue.y,
						ch->mRotationKeys[k].mValue.z
					);
					c.orientations.emplace_back(ch->mRotationKeys[k].mTime, r);
				}

				c.scalings.reserve(ch->mNumScalingKeys);
				for(int k = 0; k < ch->mNumScalingKeys; k++) {
					glm::vec3 s = {
						ch->mScalingKeys[k].mValue.x,
						ch->mScalingKeys[k].mValue.y,
						ch->mScalingKeys[k].mValue.z
					};
					c.scalings.emplace_back(ch->mScalingKeys[k].mTime, s);
				}

			}
			nameMap[name] = i;
		}
	}
	template vec3 animation::channel::get(double, std::vector<key<vec3>>const&) const;
	template quat animation::channel::get(double, std::vector<key<quat>>const&) const;

	void animation::write(std::ostream& s) const {
		s << name << std::endl;
		for(const channel& c : channels) {
			c.write(s);
		}
	}
	void animation::read(std::istream& s) {
		string line;
		std::getline(s, line);

		name = line;

		while(s && s.peek() != EOF) {
			channels.emplace_back();
			channels.back().read(s);
		}

		if(channels.size() == 0) throw std::runtime_error("Animation error animation has no channels");
		begin = channels[0].begin;
		end = channels[0].end;
		for(int i = 1; i < channels.size(); i++) {
			begin = glm::min(channels[i].begin, begin);
			end = glm::max(channels[i].end, end);
		}
	}

	bool mesh::valid() const {
		auto const psize = pos.size();
		if(!norm.empty() && norm.size() != psize) return false;
		if(!tangent.empty() && tangent.size() != psize) return false;
		if(!uv.empty() && uv.size() != psize) return false;
		if(bone0.empty()) {
			if(!(bone0.empty() && bone1.empty() && weight0.empty() && weight1.empty())) return false;
		} else {
			if(	bone0.size() != psize	||
				bone1.size() != psize	||
				weight0.size() != psize	||
				weight1.size() != psize)
				return false;
		}

		return true;
	}
	bool mesh::hasNorm() const {
		return !norm.empty();
	}
	bool mesh::hasTangent() const {
		return !tangent.empty();
	}
	bool mesh::hasUV() const {
		return !uv.empty();
	}
	bool mesh::hasBones() const {
		return !bone0.empty();
	}
	uint64_t mesh::vertSizeWithoutPadding() const {
		uint64_t vertSize = sizeof(vec3);
		if(hasNorm()) vertSize += sizeof(vec3);
		if(hasTangent()) vertSize += sizeof(vec3);
		if(hasUV()) vertSize += sizeof(vec2);
		if(hasBones()) vertSize += (sizeof(int) + sizeof(int) + sizeof(float) + sizeof(float));
		return vertSize;
	}
	uint64_t mesh::requiredMemory() const {
		return pos.size() * vertSizeWithoutPadding();
	}
	void mesh::constructContinuous(void* vdata) const {
		uint8_t* data = (uint8_t*)vdata;
		uint64_t offset = 0;
		memcpy(data + offset, pos.data(), pos.size() * sizeof(vec3));
		offset += pos.size() * sizeof(vec3);
		if(hasNorm()) {
			memcpy(data + offset, norm.data(), norm.size() * sizeof(vec3));
			offset += norm.size() * sizeof(vec3);
		}
		if(hasTangent()) {
			memcpy(data + offset, tangent.data(), tangent.size() * sizeof(vec3));
			offset += tangent.size() * sizeof(vec3);
		}
		if(hasUV()) {
			memcpy(data + offset, uv.data(), uv.size() * sizeof(vec2));
			offset += uv.size() * sizeof(vec2);
		}
		if(hasBones()) {
			memcpy(data + offset, bone0.data(), bone0.size() * sizeof(int32_t));
			offset += bone0.size() * sizeof(int32_t);
			
			memcpy(data + offset, bone1.data(), bone1.size() * sizeof(int32_t));
			offset += bone1.size() * sizeof(int32_t);
			
			memcpy(data + offset, weight0.data(), weight0.size() * sizeof(float));
			offset += weight0.size() * sizeof(float);
			
			memcpy(data + offset, weight1.data(), weight1.size() * sizeof(int32_t));
			offset += weight1.size() * sizeof(float);
		}
	}
	void mesh::constructInterleaved(void* vdata) const {
		uint8_t* data = (uint8_t*)vdata;
		uint64_t stride = vertSizeWithoutPadding();
		uint64_t voffset = 0;
		for(int i = 0; i < pos.size(); i++)
			*(vec3*)(data + i * stride) = pos[i];
		voffset += sizeof(vec3);
		
		if(hasNorm()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec3*)(data + i * stride + voffset) = norm[i];
			voffset += sizeof(vec3);
		}

		if(hasTangent()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec3*)(data + i * stride + voffset) = tangent[i];
			voffset += sizeof(vec3);
		}

		if(hasUV()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec2*)(data + i * stride + voffset) = uv[i];
			voffset += sizeof(vec2);
		}

		if(hasBones()) {
			for(int i = 0; i < pos.size(); i++) {
				*(int32_t*)(data + i * stride) = bone0[i];
			}
			voffset += sizeof(int);

			for(int i = 0; i < pos.size(); i++) {
				*(int32_t*)(data + i * stride) = bone1[i];
			}
			voffset += sizeof(int);

			for(int i = 0; i < pos.size(); i++) {
				*(float*)(data + i * stride) = weight0[i];
			}
			voffset += sizeof(float);

			for(int i = 0; i < pos.size(); i++) {
				*(float*)(data + i * stride) = weight1[i];
			}
			voffset += sizeof(float);
		}
	}
	vector<uint64_t> mesh::offsets() const {
		vector<uint64_t> res;
		
		uint64_t offset = 0;
		res.push_back(offset);
		
		offset += pos.size() * sizeof(vec3);
		if(hasNorm()) {
			res.push_back(offset);
			offset += norm.size() * sizeof(vec3);
		}
		if(hasTangent()) {
			res.push_back(offset);
			offset += tangent.size() * sizeof(vec3);
		}
		if(hasUV()) {
			res.push_back(offset);
			offset += uv.size() * sizeof(vec2);
		}
		if(hasBones()) {
			res.push_back(offset);
			offset += bone0.size() * sizeof(int32_t);
			
			res.push_back(offset);
			offset += bone1.size() * sizeof(int32_t);
			
			res.push_back(offset);
			offset += weight0.size() * sizeof(float);
			
			res.push_back(offset);
			offset += weight1.size() * sizeof(float);
		}

		return res;
	}
	
	meshDescription mesh::getDescription() const {
		meshDescription res;
		vector<VkVertexInputAttributeDescription>& attribs = res.attribs;
		vector<VkVertexInputBindingDescription>& bindings = res.bindings;
		{
			VkVertexInputAttributeDescription attrib = { };
			attrib.binding = 0;
			attrib.location = 0;
			attrib.format = VK_FORMAT_R32G32B32_SFLOAT;
			attrib.offset = 0;
			attribs.push_back(attrib);
			
			VkVertexInputBindingDescription binding = { };
			binding.binding = 0;
			binding.stride = sizeof(vec3);
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindings.push_back(binding);
		}
		if(hasNorm()) {
			VkVertexInputAttributeDescription attrib = { };
			attrib.binding = 1;
			attrib.location = 1;
			attrib.format = VK_FORMAT_R32G32B32_SFLOAT;
			attrib.offset = 0;
			attribs.push_back(attrib);
			
			VkVertexInputBindingDescription binding = { };
			binding.binding = 1;
			binding.stride = sizeof(vec3);
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindings.push_back(binding);
		}
		if(hasTangent()) {
			VkVertexInputAttributeDescription attrib = { };
			attrib.binding = 2;
			attrib.location = 2;
			attrib.format = VK_FORMAT_R32G32B32_SFLOAT;
			attrib.offset = 0;
			attribs.push_back(attrib);
			
			VkVertexInputBindingDescription binding = { };
			binding.binding = 2;
			binding.stride = sizeof(vec3);
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindings.push_back(binding);
		}
		if(hasUV()) {
			VkVertexInputAttributeDescription attrib = { };
			attrib.binding = 3;
			attrib.location = 3;
			attrib.format = VK_FORMAT_R32G32_SFLOAT;
			attrib.offset = 0;
			attribs.push_back(attrib);
			
			VkVertexInputBindingDescription binding = { };
			binding.binding = 3;
			binding.stride = sizeof(vec3);
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindings.push_back(binding);
		}
		if(hasBones()) {
			for(int i = 0; i < 4; i++) {
				VkVertexInputAttributeDescription attrib = { };
				attrib.binding = 4 + i;
				attrib.location = 4 + i;
				attrib.format = (i < 2) ? VK_FORMAT_R32_SINT : VK_FORMAT_R32_SFLOAT;
				attrib.offset = 0;
				attribs.push_back(attrib);
				
				VkVertexInputBindingDescription binding = { };
				binding.binding = 4 + i;
				binding.stride = (i < 2) ? sizeof(int32_t) : sizeof(float);
				binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				bindings.push_back(binding);
			}
		}
		return res;
	}
	uint64_t mesh::getDescriptionCode() const {
		uint64_t res = 0;
		if(hasNorm())		res |= (1 << 1);
		if(hasTangent())	res |= (1 << 2);
		if(hasUV()) 		res |= (1 << 3);
		if(hasBones())		res |= (1 << 4);
		return res;
	}

	void mesh::calculateAnimationTransforms(int animationIndex, std::vector<glm::mat4>& data, double time, behavior mode) const {
		const animationBinding& ani = animations[animationIndex];

		//localTransforms[i] corresponds to bones.bones[i]
		std::vector<glm::mat4> localTransforms(nodes.nodes.size());
		std::vector<glm::mat4> globalTransforms(nodes.nodes.size());

		//initialize initial local transforms
		for(int i = 0; i < nodes.nodes.size(); i++) {
			localTransforms[i] = nodes.nodes[i].transform;
		}

		//for each node that has an animation channel generate the correct transform
		if(mode == behavior::repeat) time = util::wrap(time, ani.ani->begin, ani.ani->end);
		for(int i = 0; i < ani.ani->channels.size(); i++) {
			const animation::channel& ch = ani.ani->channels[i];
			glm::vec3 translation = ch.getPosition(time);
			glm::quat orientation = ch.getOrientation(time);
			glm::vec3 scaling = ch.getScaling(time);
			localTransforms[ani.nodes[i]] = glm::translate(translation) * glm::toMat4(orientation) * glm::scale(scaling);
		}

		//generate subspace transforms using the hierarchy
		globalTransforms[0] = localTransforms[0]; //root node = node.nodes[0], node has no parent so global transform is local transform
		for(int i = 1; i < nodes.nodes.size(); i++) {
			globalTransforms[i] = globalTransforms[nodes.nodes[i].parent] * localTransforms[i];
		}

		//use the node transforms to generate the final bone transform
		for(int i = 0; i < bones.bones.size(); i++) {
			data[i] = globalTransforms[bones.bones[i].node] * bones.bones[i].offset;
		}
	}
	void mesh::loadMesh(const aiScene* scene, aiMesh* mesh) {
		for(size_t j = 0; j < mesh->mNumVertices; j++) {
			pos.push_back(glm::vec3(
				mesh->mVertices[j].x,
				mesh->mVertices[j].y,
				mesh->mVertices[j].z
			));
		}

		if (mesh->HasNormals()) {
			for (size_t j = 0; j < mesh->mNumVertices; j++) {
				norm.push_back(glm::vec3(
					mesh->mNormals[j].x,
					mesh->mNormals[j].y,
					mesh->mNormals[j].z
				));
			}
		}

		if (mesh->HasTangentsAndBitangents()) {
			for (size_t j = 0; j < mesh->mNumVertices; j++) {
				tangent.push_back(glm::vec3(
					mesh->mTangents[j].x,
					mesh->mTangents[j].y,
					mesh->mTangents[j].z
				));
			}
		}

		if(mesh->HasTextureCoords(0)) {
			for(size_t j = 0; j < mesh->mNumVertices; j++) {
				uv.push_back(glm::vec2(
					mesh->mTextureCoords[0][j].x,
					-mesh->mTextureCoords[0][j].y
				));
			}
		}

		for(size_t j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];

			for(size_t k = 0; k < face.mNumIndices; k++) {
				index.push_back(face.mIndices[k]);
			}
		}
	}
	void mesh::loadNodes(const aiScene* scene, aiMesh* mesh) {
		nodes.loadAllNodes(scene);
	}
	void mesh::loadBones(const aiScene* scene, aiMesh* mesh) {
		bones.loadAllBones(scene, nodes);

		for(int j = 0; j < pos.size(); j++) {
			vertexBoneInfo info = bones.getInfo(j);
			info.normalize();
			bone0.push_back(info.primary);
			bone1.push_back(info.secondary);
			weight0.push_back(info.primaryWeight);
			weight1.push_back(info.secondaryWeight);
		}
	}
	
	bool mesh::bindAnimation(const animation& ani) {
			animationBinding binding;
			binding.ani = &ani;
			binding.nodes.resize(ani.channels.size());
			for(int i = 0; i < ani.channels.size(); i++) {
				const auto& it = nodes.nameMap.find(ani.channels[i].nodeName);
				if(it == nodes.nameMap.end()) return false;
				binding.nodes[i] = it->second;
			}
			animations.emplace_back(std::move(binding));
			return true;
		}
	
	void mesh::convertAnimationFromCollada(std::string location, std::string outLocation) {
		Assimp::Importer imp;
		//imp.SetExtraVerbose(true);

		const aiScene* scene = imp.ReadFile(location.c_str(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);
		if(!scene) {
			util::sout(imp.GetErrorString());
			throw std::runtime_error(("Failed to load model " + location).c_str());
		}

		animationContainer rawAnimation;
		rawAnimation.loadAllAnimations(scene);

		if(rawAnimation.animations.size() != 1) {
			throw std::runtime_error("Must be only one animation in collada file");
		}
		std::ofstream output(outLocation);
		rawAnimation.animations[0].write(output);
	}

	mesh::mesh() {
	}
	mesh::mesh(std::string path) {
		Assimp::Importer imp;
		imp.SetExtraVerbose(true);

		const aiScene* scene = imp.ReadFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace);
		if(!scene) {
			util::sout(imp.GetErrorString());
			throw std::runtime_error(("Failed to load model " + path).c_str());
		}

		size_t numMeshes = scene->mNumMeshes;
		if(numMeshes != 1) throw std::runtime_error("Error, multiple meshes in collada file");

		aiMesh* mesh = scene->mMeshes[0];
		std::string meshname(mesh->mName.C_Str());

		loadMesh(scene, mesh);

		loadNodes(scene, mesh);

		loadBones(scene, mesh);

		imp.FreeScene();
	}
	string meshDescription::toString() const {
		string res;
		for (int i = 0; i < attribs.size(); i++) {
			res += "layout(location = " + std::to_string(attribs[i].location) + ") in [...] binding" + std::to_string(attribs[i].binding) + ";\n";
		}
		return res;
	}
	meshDescription meshDescription::getLit(bool rigged) {
		mesh m;
		m.pos.push_back(vec3(0, 0, 0));
		m.norm.push_back(vec3(0, 0, 0));
		m.tangent.push_back(vec3(0, 0, 0));
		m.uv.push_back(vec2(0, 0));
		if(rigged) m.bone0.push_back(0);
		return m.getDescription();
	}
	meshDescription meshDescription::getShadeless(bool rigged) {
		mesh m;
		m.pos.push_back(vec3(0, 0, 0));
		m.uv.push_back(vec2(0, 0));
		if (rigged) m.bone0.push_back(0);
		return m.getDescription();
	}
}