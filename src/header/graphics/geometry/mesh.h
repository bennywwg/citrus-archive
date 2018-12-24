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
	class mesh {
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

		mesh(std::string location) {
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
		
	class model {
		graphics::vertexView3f* vertices;
		graphics::vertexView3f* normals;
		graphics::vertexView2f* uvs;
		graphics::indexViewui* indices;

		graphics::vertexArray* vao;

	public:void draw() {
			vao->drawAll();
		}

		model(const mesh& mesh, graphics::buffer* buf) {
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
		~model() {
			if (vertices) delete vertices;
			if (normals) delete normals;
			if (uvs) delete uvs;
			if (indices) delete indices;
			if (vao) delete vao;
		}

		NO_COPY(model);
	};

}

#endif