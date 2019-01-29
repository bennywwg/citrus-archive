#include <graphics/vulkan/ctmesh.h>

namespace citrus::graphics {
	static glm::mat4 node::convertMat(const aiMatrix4x4& m) {
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

	void nodeContainer::addNodes(const aiNode* n, int parent) {
		int ci = nodes.size();
		nodes.emplace_back();
		nodes[ci].name = n->mName.C_Str();
		nodes[ci].parent = parent;
		nodes[ci].transform = node::convertMat(n->mTransformation);
		nodes[ci].children.resize(n->mNumChildren);
		for(int i = 0; i < n->mNumChildren; i++) {
			int childIndex = addNodes(n->mChildren[i], ci);
			nodes[ci].children[i] = childIndex;
		}
		return ci;
	}
	nodeContainer::nodeContainer(const aiScene* scene) {
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
			bone.offset = node::convertMat(inBone.mOffsetMatrix);
			weights.emplace_back(scene->mMeshes[0]->mNumVertices, -1.0f);
			for (size_t k = 0; k < inBone.mNumWeights; k++) {
				weights[j][inBone.mWeights[k].mVertexId] = inBone.mWeights[k].mWeight;
			}

			nameMap[name] = j;
		}
	}

	template<typename T>
	static T animation::key<T>::mix(const double& time, const key<T>& l, const key<T>& r) {
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

	bool ctmesh::valid() const {
		auto const psize = pos.size();
		if(!color.empty() && color.size() != psize) return false;
		if(!norm.empty() && norm.size() != psize) return false;
		if(!tangnet.empty() && tangnet.size() != psize) return false;
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
	bool ctmesh::hasColor() const {
		return !color.empty();
	}
	bool ctmesh::hasNorm() const {
		return !norm.empty();
	}
	bool ctmesh::hasTangent() const {
		return !tangent.empty();
	}
	bool ctmesh::hasUV() const {
		return !uv.empty();
	}
	bool ctmesh::hasBones() const {
		return !bone0.empty();
	}
	uint64_t ctmesh::vertSizeWithoutPadding() const {
		uint64_t vertSize = sizeof(vec3);
		if(hasColor()) vertSize += sizeof(vec3);
		if(hasNorm()) vertSize += sizeof(vec3);
		if(hasTangent()) vertSize += sizeof(vec3);
		if(hasUV()) vertSize += sizeof(vec2);
		if(hasBones()) vertSize += (sizeof(int) + sizeof(int) + sizeof(float) + sizeof(float));
		return vertSize;
	}
	uint64_t ctmesh::requiredMemory() const {
		return pos.size() * vertSizeWithoutPadding();
	}
	void ctmesh::construct(void* vdata) const {
		uint8_t* data = (uint8_t*)vdata;
		uint64_t stride = vertSizeWithoutPadding();
		uint64_t voffset = 0;
		for(int i = 0; i < pos.size(); i++)
			*(vec3*)(data + i * stride) = pos[i];
		voffset += sizeof(vec3);

		if(hasColor()) {
			for(int i = 0; i < pos.size(); i++)
				*(vec3*)(data + i * stride + voffset) = color[i];
			voffset += sizeof(vec3);
		}

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

	ctmesh ctmesh::loadCOLLADA(std::string path) {

	}
}