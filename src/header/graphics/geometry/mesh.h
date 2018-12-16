#pragma once
#ifndef MESH_H
#define MESH_H

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

namespace citrus {
	namespace geom {
		struct boneNode {
			boneNode* parent;
			std::map<std::string, boneNode> children;

			std::string name;
			glm::mat4 transform;
			glm::mat4 accumulatedTransform;

			static glm::mat4 convertMat(aiMatrix4x4 m) {
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

			void computeAllAccumulated() {
				if (parent) {
					accumulatedTransform = transform * parent->accumulatedTransform;
				} else {
					accumulatedTransform = transform;
				}
				for (auto& child : children) {
					child.second.computeAllAccumulated();
				}
			}
			std::vector<boneNode> allBones() {
				std::vector<boneNode> res;
				accumulateAllBones(res);
				return res;
			}
			void accumulateAllBones(std::vector<boneNode>& res) {
				res.push_back(*this);
				for (auto& child : children) {
					child.second.accumulateAllBones(res);
				}
			}


			boneNode() = default;
			boneNode(aiNode* node) : boneNode(node, nullptr, this) {}
		private:
			boneNode(aiNode* node, boneNode* _parent, boneNode* _root) {
				name = node->mName.C_Str();
				transform = convertMat(node->mTransformation);
				parent = _parent;
				for (size_t i = 0; i < node->mNumChildren; i++) {
					children[std::string(node->mChildren[i]->mName.C_Str())] = boneNode(node->mChildren[i], this, _root);
				}
			}
		};

		struct bone {
			std::string name;
			glm::mat4 offset;
			std::map<unsigned int, float> weights;
		};

		struct vertexBoneInfo {
			unsigned int primary = -1, secondary = -1;
			float primaryWeight = 0.0f, secondaryWeight = 0.0f;
			void normalize() {
				if (primary == -1) {
					primaryWeight = 1.0f;
				} else {
					float total = primaryWeight + secondaryWeight;
					primaryWeight /= total;
					secondaryWeight /= total;
				}
			}
		};

		class boneContainer {
		public:
			std::vector<bone> bones;

			std::vector<glm::mat4> getAnimationTransforms() {

			}

			vertexBoneInfo getInfo(unsigned int vertex) {
				vertexBoneInfo res;
				for (int i = 0; i < bones.size(); i++) {
					bone& bone = bones[i];

					if (bone.weights.find(vertex) != bone.weights.end()) {
						float weight = bone.weights.find(vertex)->second;
						if (weight > res.primaryWeight) {
							res.secondary = res.primary;
							res.secondaryWeight = res.primaryWeight;
							res.primary = i;
							res.primaryWeight = weight;
						} else if (weight > res.secondaryWeight) {
							res.secondary = i;
							res.secondaryWeight = weight;
						}
					}

				}
				return res;
			}
		};

		class simpleMesh {
		public:
			std::vector<glm::vec3> pos;
			std::vector<glm::vec3> normal;
			std::vector<glm::vec2> uv;
			std::vector<unsigned int> index;

			size_t requiredSize() const {
				return
					pos.size() * sizeof(glm::vec3) +
					normal.size() * sizeof(glm::vec3) +
					uv.size() * sizeof(glm::vec2) +
					index.size() * sizeof(unsigned int);
			}

			simpleMesh(std::string location) {
				Assimp::Importer imp;

				const aiScene* scene = imp.ReadFile(location,
					aiProcess_CalcTangentSpace |
					aiProcess_Triangulate |
					aiProcess_JoinIdenticalVertices |
					aiProcess_SortByPType);

				imp.SetExtraVerbose(true);

				std::string verboseError = imp.GetErrorString();
				if (!scene) throw std::exception(("Failed to load mesh - scene did not load\n" + verboseError).c_str());

				size_t numMeshes = scene->mNumMeshes;
				if (numMeshes != 1) throw std::exception("Failed to load mesh - there must be only one mesh in the scene");
				for (size_t i = 0; i < numMeshes; i++) {
					aiMesh* mesh = scene->mMeshes[i];

					std::string meshname(mesh->mName.C_Str());

					for (size_t j = 0; j < mesh->mNumVertices; j++) {
						pos.push_back(glm::vec3(
							mesh->mVertices[j].x,
							mesh->mVertices[j].y,
							mesh->mVertices[j].z
						));
						uv.push_back(glm::vec2(
							mesh->mTextureCoords[0][j].x,
							-mesh->mTextureCoords[0][j].y
						));
					}

					for (size_t j = 0; j < mesh->mNumFaces; j++) {
						const aiFace& face = mesh->mFaces[j];

						for (size_t k = 0; k < face.mNumIndices; k++) {
							index.push_back(face.mIndices[k]);
						}
					}
				}
			}
		};
		
		class simpleModel {
			graphics::vertexView3f* vertices;
			graphics::vertexView3f* normals;
			graphics::vertexView2f* uvs;
			graphics::indexViewui* indices;

			graphics::vertexArray* vao;

		public:
			const simpleMesh mesh;

			void draw() {
				vao->drawAll();
			}

			simpleModel(const simpleMesh& mesh, graphics::buffer* buf) : mesh(mesh) {
				size_t start = 0;
				vertices = new graphics::vertexView3f(buf, start, mesh.pos.size(), mesh.pos.data());
				start += mesh.pos.size() * sizeof(glm::vec3);

				normals = new graphics::vertexView3f(buf, start, mesh.normal.size(), mesh.normal.data());
				start += mesh.normal.size() * sizeof(glm::vec3);

				uvs = new graphics::vertexView2f(buf, start, mesh.uv.size(), mesh.uv.data());
				start += mesh.uv.size() * sizeof(glm::vec2);

				indices = new graphics::indexViewui(buf, start, mesh.index.size(), mesh.index.data());

				vao = new graphics::vertexArray({
					graphics::vertexAttribute(0, false, *vertices),
					graphics::vertexAttribute(1, false, *normals),
					graphics::vertexAttribute(2, false, *uvs),
				}, *indices);
			}
			~simpleModel() {
				if (vertices) delete vertices;
				if (normals) delete normals;
				if (uvs) delete uvs;
				if (indices) delete indices;
				if (vao) delete vao;
			}

			NO_COPY(simpleModel);
		};

		class conventionalmesh {
		public:
			std::vector<glm::vec3> pos;
			std::vector<glm::vec3> normal;
			std::vector<glm::vec2> uv;
			std::vector<unsigned int> index;
			std::vector<unsigned int> bone0, bone1;
			std::vector<float> weight0, weight1;

			boneContainer bones;
			boneNode boneBase;

			uint32_t requiredMemory() {
				return 0
					+ pos.size() * sizeof(glm::vec3)
					+ normal.size() * sizeof(glm::vec3)
					+ uv.size() * sizeof(glm::vec2)
					+ index.size() * sizeof(unsigned int)
					+ bone0.size() * sizeof(unsigned int)
					+ bone1.size() * sizeof(unsigned int)
					+ weight0.size() * sizeof(float)
					+ weight1.size() * sizeof(float);
			}

			conventionalmesh(std::string location) {
				Assimp::Importer imp;

				const aiScene* scene = imp.ReadFile(location,
					aiProcess_CalcTangentSpace |
					aiProcess_Triangulate |
					aiProcess_JoinIdenticalVertices |
					aiProcess_SortByPType);

				imp.SetExtraVerbose(true);

				util::sout(imp.GetErrorString());

				boneBase = boneNode(scene->mRootNode);

				size_t numMeshes = scene->mNumMeshes;
				for (size_t i = 0; i < numMeshes; i++) {
					aiMesh* mesh = scene->mMeshes[i];

					std::string meshname(mesh->mName.C_Str());

					for (size_t j = 0; j < mesh->mNumVertices; j++) {
						pos.push_back(glm::vec3(
							mesh->mVertices[j].x,
							mesh->mVertices[j].y,
							mesh->mVertices[j].z
						));
						uv.push_back(glm::vec2(
							mesh->mTextureCoords[0][j].x,
							-mesh->mTextureCoords[0][j].y
						));
					}

					for (size_t j = 0; j < mesh->mNumFaces; j++) {
						const aiFace& face = mesh->mFaces[j];
						
						for (size_t k = 0; k < face.mNumIndices; k++) {
							index.push_back(face.mIndices[k]);
						}
					}

					for (size_t j = 0; j < mesh->mNumBones; j++) {
						const aiBone& inBone = *mesh->mBones[j];
						
						std::string name(inBone.mName.data);
						util::sout("Bone: " + name + "\n");

						bone theBone;
						theBone.name = inBone.mName.C_Str();
						aiMatrix4x4 m = inBone.mOffsetMatrix;
						theBone.offset = boneNode::convertMat(m);
						for (size_t k = 0; k < inBone.mNumWeights; k++) {
							theBone.weights[inBone.mWeights[k].mVertexId] = inBone.mWeights[k].mWeight;
						}

						bones.bones.push_back(theBone);
					}

					for (int j = 0; j < pos.size(); j++) {
						vertexBoneInfo info = bones.getInfo(j);	
						info.normalize();
						bone0.push_back(info.primary);
						bone1.push_back(info.secondary);
						weight0.push_back(info.primaryWeight);
						weight1.push_back(info.secondaryWeight);
					}

					for(int j = 0; j < scene->mNumAnimations; j++) {
						std::string animName = scene->mAnimations[j]->mName.data;
						util::sout("Animation: " + animName + "\n");
						//scene->mAnimations[0]->
						//scene->mAnimations[j]->mMeshChannels[0]->mKeys[0].
					}

					
				}
			}
		 };

		class riggedModel {
		public:
			graphics::vertexView3f* vertices = nullptr;
			graphics::vertexView3f* normals = nullptr;
			graphics::vertexView2f* uvs = nullptr;
			graphics::vertexView1ui* bones0 = nullptr;
			graphics::vertexView1ui* bones1 = nullptr;
			graphics::vertexView1f* weights0 = nullptr;
			graphics::vertexView1f* weights1 = nullptr;
			graphics::indexViewui* indices = nullptr;

			riggedModel(const conventionalmesh& mesh, graphics::buffer* buf) {
				size_t start = 0;
				vertices = new graphics::vertexView3f(buf, start, mesh.pos.size(), mesh.pos.data());
				start += mesh.pos.size() * sizeof(glm::vec3);

				uvs = new graphics::vertexView2f(buf, start, mesh.uv.size(), mesh.uv.data());
				start += mesh.uv.size() * sizeof(glm::vec2);

				bones0 = new graphics::vertexView1ui(buf, start, mesh.bone0.size(), mesh.bone0.data());
				start += mesh.bone0.size() * sizeof(unsigned int);

				bones1 = new graphics::vertexView1ui(buf, start, mesh.bone1.size(), mesh.bone1.data());
				start += mesh.bone1.size() * sizeof(unsigned int);

				weights0 = new graphics::vertexView1f(buf, start, mesh.weight0.size(), mesh.weight0.data());
				start += mesh.weight0.size() * sizeof(float);

				weights1 = new graphics::vertexView1f(buf, start, mesh.weight1.size(), mesh.weight1.data());
				start += mesh.weight1.size() * sizeof(float);

				indices = new graphics::indexViewui(buf, start, mesh.index.size(), mesh.index.data());
			}
			~riggedModel() {
				if (vertices) delete vertices;
				if (normals) delete normals;
				if (uvs) delete uvs;
				if (bones0) delete bones0;
				if (bones1) delete bones1;
				if (weights0) delete weights0;
				if (weights1) delete weights1;
				if (indices) delete indices;
			}
		private:
			riggedModel(const riggedModel&) = delete;
			riggedModel& operator=(const riggedModel&) = delete;
		};
	}
}

#endif