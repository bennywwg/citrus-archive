#include <engine/elements/worldManager.h>
#include <engine/elements/rigidBodyComponent.h>
#include <engine/engine.h>
#include <engine/manager.inl>
#include <engine/elementRef.inl>

namespace citrus::engine {
	void worldManager::preRender() {
		auto list = e->getAllOfType<rigidBodyComponent>();
		for(auto& rbc : list) {
			if(rbc->ent.getGlobalTransform() != rbc->body->getTransform()) {
				rbc->body->setTransform(rbc->ent.getGlobalTransform());
			}
		}
		w->step();
	}
}
