#include <engine/elements/meshFilter.h>

#include <engine/elements/meshManager.h>
#include <engine/elements/shaderManager.h>
#include <engine/elements/textureManager.h>
#include <engine/elements/freeCam.h>

#include <engine/manager.inl>

#include <engine/elementRef.inl>

namespace citrus::engine {
	void meshFilter::setMeshByName(std::string name) {
		eleRef<meshManager> man = e->getAllOfType<meshManager>()[0];
		model = man->getModel(name);
	}
	void meshFilter::setTextureByName(std::string name) {
		eleRef<textureManager> man = e->getAllOfType<textureManager>()[0];
		tex = man->getTexture(name);
	}
	void meshFilter::setShaderByName(std::string name) {
		eleRef<shaderManager> man = e->getAllOfType<shaderManager>()[0];
		sh = man->getShader(name);
	}
	void meshFilter::render() {
		eleRef<freeCam> cam = e->getAllOfType<freeCam>()[0];

		if(sh) sh->use();
		if(sh && tex) sh->setSampler("tex", *tex);
		if(sh) sh->setUniform("projectionMat", cam->cam.getProjectionMatrix());
		if(sh) sh->setUniform("viewMat", cam->cam.getViewMatrix());
		if(sh) sh->setUniform("modelMat", glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));
		if(sh) sh->setUniform("modelViewProjectionMat", cam->cam.getViewProjectionMatrix());
		if(sh) sh->setUniform("viewBone", 0);
		if(model) model->drawAll();
	}
	meshFilter::meshFilter(entityRef ent) : element(ent, typeid(meshFilter)), model(nullptr), sh(nullptr), tex(nullptr) { }
}