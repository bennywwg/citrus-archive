#pragma once

#include <vector>
#include <map>

#include <glm/ext.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <graphics/buffer/buffer.inl>

#include <util/util.h>
#include <util/stdUtil.h>

namespace citrus::geom {
	enum transformCoordinates {
		xyz,
		xzy
	};

	struct vertexBoneInfo {
		int primary = -1, secondary = -1;
		float primaryWeight = 0.0f, secondaryWeight = 0.0f;
		void normalize() {
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
	};

	struct node {
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
		std::string name;
		glm::mat4 transform;
		int parent;
		std::vector<int> children;
	};

	struct nodeContainer {
		std::vector<node> nodes;
		std::map<string, int> nameMap;

	private:
		inline int addNodes(const aiNode* n, int parent = -1) {
			int ci = nodes.size();
			if(ci == 7) {
				int v = 5;
			}
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

	public:
		inline void loadAllNodes(const aiScene* scene) {
			addNodes(scene->mRootNode);
			for(int i = 0; i < nodes.size(); i++) {
				nameMap[nodes[i].name] = i;
			}
		}
	};

	struct bone {
		int node;
		glm::mat4 offset;
	};

	struct boneContainer {
		std::vector<bone> bones;
		std::map<string, int> nameMap;
		std::vector<std::vector<float>> weights;

		inline vertexBoneInfo getInfo(unsigned int vertex) {
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

		inline void loadAllBones(const aiScene* scene, const nodeContainer& nodes) {
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
	};

	enum behavior {
		base = 0,
		nearest = 1,
		repeat = 3,
		linear = 2
	};

	struct animation {
		template<typename T>
		struct key {
			double time;
			T val;
			inline static T mix(const double& time, const key<T>& l, const key<T>& r) {
				return glm::mix(l.val, r.val, (float)((time - l.time) / (r.time - l.time)));
			}
			inline key(const double& time, const T& val) : time(time), val(val) { }
		};

		struct channel {
			string nodeName;
			double begin, end;
			std::vector<key<glm::vec3>> positions;
			std::vector<key<glm::quat>> orientations;
			std::vector<key<glm::vec3>> scalings;

			template<typename T>
			inline T get(double time, std::vector<key<T>>const& keys) const {
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

			inline glm::vec3 getPosition(double time) const {
				return get(time, positions);
			}
			inline glm::quat getOrientation(double time) const {
				return get(time, orientations);
			}
			inline glm::vec3 getScaling(double time) const {
				return get(time, scalings);
			}

			inline void write(std::ostream& s) const {
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
			inline void read(std::istream& s) {
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
		};

		string name;
		double begin, end;
		std::vector<channel> channels;

		inline void write(std::ostream& s) const {
			s << name << std::endl;
			for(const channel& c : channels) {
				c.write(s);
			}
		}
		inline void read(std::istream& s) {
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
	};

	//this references an animation
	//it has a list of node indices instead of
	//node names so it can be used to animate
	//nodes[i] corresponds to ani->channels[i]
	struct animationBinding {
		const animation* ani;
		std::vector<int> nodes;
	};

	struct animationContainer {
		std::vector<animation> animations;
		std::map<string, int> nameMap;

		inline void loadAllAnimations(const aiScene* scene) {
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
	};
	

	class animesh {
		public:
		string name = "Default Mesh Name";

		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec2> uv;
		std::vector<unsigned int> index;
		std::vector<int> bone0, bone1;
		std::vector<float> weight0, weight1;

		nodeContainer nodes;
		boneContainer bones;
		std::vector<animationBinding> animations;

		inline uint32_t requiredMemory() {
			return 0
				+ pos.size() * sizeof(glm::vec3)
				+ normal.size() * sizeof(glm::vec3)
				+ uv.size() * sizeof(glm::vec2)
				+ index.size() * sizeof(unsigned int)
				+ bone0.size() * sizeof(int)
				+ bone1.size() * sizeof(int)
				+ weight0.size() * sizeof(float)
				+ weight1.size() * sizeof(float);
		}

		inline void calculateAnimationTransforms(int animationIndex, std::vector<glm::mat4>& data, double time, behavior mode) const {
			const animationBinding& ani = animations[animationIndex];

			//localTransforms[i] corresponds to bones.bones[i]
			std::vector<glm::mat4> localTransforms(nodes.nodes.size());
			std::vector<glm::mat4> globalTransforms(nodes.nodes.size());

			//initialize initial local transforms
			for(int i = 0; i < nodes.nodes.size(); i++) {
				localTransforms[i] = nodes.nodes[i].transform;
			}

			//for each node that has an animation channel generate the correct transform
			if(mode == repeat) time = util::wrap(time, ani.ani->begin, ani.ani->end);
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

		inline void loadMesh(const aiScene* scene, aiMesh* mesh, transformCoordinates tr = xyz) {
			for(size_t j = 0; j < mesh->mNumVertices; j++) {
				pos.push_back(glm::vec3(
					mesh->mVertices[j].x,
					(tr == xyz) ? mesh->mVertices[j].y : mesh->mVertices[j].z,
					(tr == xyz) ? mesh->mVertices[j].z : mesh->mVertices[j].y
				));
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
		inline void loadNodes(const aiScene* scene, aiMesh* mesh, transformCoordinates tr = xyz) {
			nodes.loadAllNodes(scene);
		}
		inline void loadBones(const aiScene* scene, aiMesh* mesh) {
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

		inline bool bindAnimation(const animation& ani) {
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

		inline static void convertAnimationFromCollada(std::string location, std::string outLocation) {
			Assimp::Importer imp;
			//imp.SetExtraVerbose(true);

			const aiScene* scene = imp.ReadFile(location,
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

		inline animesh(std::string location, transformCoordinates tr = xyz) {
			Assimp::Importer imp;
			//imp.SetExtraVerbose(true);

			const aiScene* scene = imp.ReadFile(location,
				aiProcess_CalcTangentSpace |
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType);
			if(!scene) {
				util::sout(imp.GetErrorString());
				throw std::runtime_error(("Failed to load model " + location).c_str());
			}

			size_t numMeshes = scene->mNumMeshes;
			if(numMeshes != 1) throw std::runtime_error("Error, multiple meshes in collada file");

			aiMesh* mesh = scene->mMeshes[0];
			std::string meshname(mesh->mName.C_Str());

			loadMesh(scene, mesh, tr);

			loadNodes(scene, mesh, tr);

			loadBones(scene, mesh);

			imp.FreeScene();
		}
	};

	class animodel {
		public:
		graphics::vertexView3f* vertices = nullptr;
		graphics::vertexView3f* normals = nullptr;
		graphics::vertexView2f* uvs = nullptr;
		graphics::vertexView1i* bones0 = nullptr;
		graphics::vertexView1i* bones1 = nullptr;
		graphics::vertexView1f* weights0 = nullptr;
		graphics::vertexView1f* weights1 = nullptr;
		graphics::indexViewui* indices = nullptr;

		animodel(const animesh& mesh, graphics::buffer* buf) {
			size_t start = 0;
			vertices = new graphics::vertexView3f(buf, start, mesh.pos.size(), mesh.pos.data());
			start += mesh.pos.size() * sizeof(glm::vec3);

			uvs = new graphics::vertexView2f(buf, start, mesh.uv.size(), mesh.uv.data());
			start += mesh.uv.size() * sizeof(glm::vec2);

			bones0 = new graphics::vertexView1i(buf, start, mesh.bone0.size(), mesh.bone0.data());
			start += mesh.bone0.size() * sizeof(unsigned int);

			bones1 = new graphics::vertexView1i(buf, start, mesh.bone1.size(), mesh.bone1.data());
			start += mesh.bone1.size() * sizeof(unsigned int);

			weights0 = new graphics::vertexView1f(buf, start, mesh.weight0.size(), mesh.weight0.data());
			start += mesh.weight0.size() * sizeof(float);

			weights1 = new graphics::vertexView1f(buf, start, mesh.weight1.size(), mesh.weight1.data());
			start += mesh.weight1.size() * sizeof(float);

			indices = new graphics::indexViewui(buf, start, mesh.index.size(), mesh.index.data());
		}
		~animodel() {
			if(vertices) delete vertices;
			if(normals) delete normals;
			if(uvs) delete uvs;
			if(bones0) delete bones0;
			if(bones1) delete bones1;
			if(weights0) delete weights0;
			if(weights1) delete weights1;
			if(indices) delete indices;
		}
		private:
		animodel(const animodel&) = delete;
		animodel& operator=(const animodel&) = delete;
	};
}