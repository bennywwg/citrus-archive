#pragma once

#include <vector>
#include <map>

#include <glm\ext.hpp>

#include <assimp\Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <graphics/buffer/vertexView.h>
#include <graphics/buffer/indexView.h>
#include <graphics/buffer/buffer.h>
#include <graphics/buffer/bufferView.h>
#include <graphics/geometry/vertexarray.h>

#include <util/util.h>
#include <util/stdUtil.h>

namespace citrus::geom {
	enum transformCoordinates {
		xyz,
		xzy
	};

	struct vertexBoneInfo {
		unsigned int primary = -1, secondary = -1;
		float primaryWeight = 0.0f, secondaryWeight = 0.0f;
		void normalize() {
			if(primary == -1) {
				primaryWeight = 1.0f;
			} else {
				float total = primaryWeight + secondaryWeight;
				primaryWeight /= total;
				secondaryWeight /= total;
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
		std::map<unsigned int, float> weights;
	};

	struct boneContainer {
		std::vector<bone> bones;
		std::map<string, int> nameMap;

		inline vertexBoneInfo getInfo(unsigned int vertex) {
			vertexBoneInfo res;
			for(int i = 0; i < bones.size(); i++) {
				bone& bone = bones[i];

				if(bone.weights.find(vertex) != bone.weights.end()) {
					float weight = bone.weights.find(vertex)->second;
					if(weight > res.primaryWeight) {
						res.secondary = res.primary;
						res.secondaryWeight = res.primaryWeight;
						res.primary = i;
						res.primaryWeight = weight;
					} else if(weight > res.secondaryWeight) {
						res.secondary = i;
						res.secondaryWeight = weight;
					}
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
				for (size_t k = 0; k < inBone.mNumWeights; k++) {
					bone.weights[inBone.mWeights[k].mVertexId] = inBone.mWeights[k].mWeight;
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
			int node;
			double duration; //inhereted from animation
			behavior preState, postState;
			std::vector<key<glm::vec3>> positions;
			std::vector<key<glm::quat>> orientations;
			std::vector<key<glm::vec3>> scalings;

			template<typename T>
			inline T get(double time, std::vector<key<T>>const& keys) const {
				if(time < 0) {
					if(preState == behavior::base) {
						return keys.front().val;
					} else if(preState == behavior::nearest) {
						return keys.front().val;
					} else if(preState == behavior::repeat) {
						time = util::rmod(time, duration);
					} else if(preState == behavior::linear) {
						if(keys.size() == 1) {
							return keys.front().val;
						} else {
							return key<T>::mix(time, keys.front(), keys[1]);
						}
					}
				} else if(time >= duration) {
					if(preState == behavior::base) {
						return keys.back().val;
					} else if(preState == behavior::nearest) {
						return keys.back().val;
					} else if(preState == behavior::repeat) {
						time = util::rmod(time, duration);
					} else if(preState == behavior::linear) {
						if(keys.size() == 1) {
							return keys.back().val;
						} else {
							return key<T>::mix(time, keys[keys.size() - 2], keys.back());
						}
					}
				}
				if(keys.size() == 1) {
					return keys.front().val;
				} else {
					for(int i = 0; i < keys.size() - 1; i++) {
						if(time >= keys[i].time && time < keys[i + 1].time) {
							return key<T>::mix(time, keys[i], keys[i + 1]);
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
		};

		string name;
		double duration;
		std::vector<channel> channels;
	};

	struct animationContainer {
		std::vector<animation> animations;
		std::map<string, int> nameMap;

		inline void loadAllAnimations(const aiScene* scene, const nodeContainer& nodes) {
			for(int i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* ani = scene->mAnimations[i];
				std::string name = ani->mName.C_Str();

				animations.emplace_back();
				animation& a = animations.back();
				a.name = name;
				a.duration = ani->mDuration;

				for(int j = 0; j < ani->mNumChannels; j++) {
					aiNodeAnim* ch = ani->mChannels[j];

					string nodeName = ch->mNodeName.C_Str();
					auto nodeIndexIt = nodes.nameMap.find(nodeName);
					if(nodeIndexIt == nodes.nameMap.end()) throw std::runtime_error("Mesh error can't find node associated with channel");

					a.channels.emplace_back();
					animation::channel& c = a.channels.back();

					c.node = nodeIndexIt->second;
					c.duration = a.duration;
					c.preState = (behavior)ch->mPreState; c.preState = repeat;
					c.postState = (behavior)ch->mPostState; c.postState = repeat;

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
		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec2> uv;
		std::vector<unsigned int> index;
		std::vector<int> bone0, bone1;
		std::vector<float> weight0, weight1;

		nodeContainer nodes;
		boneContainer bones;
		animationContainer animations;

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

		inline void calculateAnimationTransforms(int animationIndex, std::vector<glm::mat4>& data, double time) const {
			const animation& ani = animations.animations[animationIndex];

			//localTransforms[i] corresponds to bones.bones[i]
			std::vector<glm::mat4> localTransforms(nodes.nodes.size());
			std::vector<glm::mat4> globalTransforms(nodes.nodes.size());

			//initialize initial local transforms
			for(int i = 0; i < nodes.nodes.size(); i++) {
				localTransforms[i] = nodes.nodes[i].transform;
			}

			//for each node that has an animation channel generate the correct transform
			for(int i = 0; i < ani.channels.size(); i++) {
				const animation::channel& ch = ani.channels[i];
				glm::vec3 translation = ch.getPosition(time);
				glm::quat orientation = ch.getOrientation(time);
				glm::vec3 scaling = ch.getScaling(time);
				localTransforms[ch.node] = glm::translate(translation) * glm::toMat4(orientation) * glm::scale(scaling);
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
				uv.push_back(glm::vec2(
					mesh->mTextureCoords[0][j].x,
					-mesh->mTextureCoords[0][j].y
				));
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
		inline void loadAnimations(const aiScene* scene, aiMesh* mesh) {
			animations.loadAllAnimations(scene, nodes);
		}

		inline animesh(std::string location, transformCoordinates tr = xyz) {
			Assimp::Importer imp;
			imp.SetExtraVerbose(true);

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

			loadAnimations(scene, mesh);

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