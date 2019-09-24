#include "citrus/util.h"

#include "citrus/graphics/system/instance.h"

#include <iomanip>

#include "citrus/engine/engine.h"
#include "citrus/engine/freeCam.h"
#include "citrus/engine/worldManager.h"
#include "citrus/engine/meshFilter.h"
#include "citrus/engine/rigidBodyComponent.h"
#include "citrus/engine/playerController.h"
#include "citrus/engine/worldManager.h"
#include "citrus/engine/rigidBodyComponent.h"
#include "citrus/engine/projectile.h"
#include "citrus/engine/sensorEle.h"

#include "citrus/engine/entityRef.inl"
#include "citrus/engine/elementRef.inl"
#include "citrus/engine/manager.inl"
#include "citrus/graphics/image.h"
using citrus::engine::entityRef;

#include <sstream>


using namespace citrus;

void initializeGLFW() {
	if (!glfwInit()) throw std::runtime_error("Unable to glfwInit()");
}
void terminateGLFW() {
	glfwTerminate();
}

int main(int argc, char **argv) {
	std::string resDir = argc == 1 ? "" :argv[1];

	if (argc >= 3 && (string(argv[2]) == "compileShaders")) {
		util::compileAllShaders(fpath(resDir) / "shaders");
	}

	util::sout("Citrus 0.0.0 - DO NOT DISTRIBUTE\n");
	
	initializeGLFW();
	
	{
		engine::engine e(1.0 / 100.0);
		e.resDir = resDir;
		
		e.man->registerType<engine::freeCam>("Free Cam", true);
		e.man->registerType<engine::meshFilter>("Mesh Filter", true);
		e.man->registerType<engine::playerController>("Player Controller", true);
		e.man->registerType<engine::worldManager>("World Manager", true);
		e.man->registerType<engine::rigidBodyComponent>("Rigid Body", false);
		e.man->registerType<engine::sensorEle>("Sensor", false);
		e.man->registerType<engine::projectile>("Projectile", true);
		e.man->setOrder({
			typeid(engine::worldManager),
			typeid(engine::sensorEle),
			typeid(engine::rigidBodyComponent),
			typeid(engine::freeCam),
			typeid(engine::playerController),
			typeid(engine::projectile),
			typeid(engine::meshFilter)
		});

		entityRef cam2 = e.man->create("Test 2", {
			engine::eleInit<engine::freeCam>::run(
				[](engine::freeCam& c) {
					c.cam.aspectRatio = 16.0f / 9.0f;
					c.cam.zFar = 1500.0f;
					c.cam.verticalFOV = 120.0f;
				}
			)
		}, util::nextID());

		e.man->create("Physics", {engine::eleInit<engine::worldManager>()}, util::nextID());

		auto ent2 = e.man->create("Player", {
			engine::eleInit<engine::playerController>(),
			engine::eleInit<engine::rigidBodyComponent>::run([](engine::rigidBodyComponent& cmp) {
				//rb.body->dynamic = false;
				cmp.setToBox(glm::vec3(0.4f, 0.7f, 0.3f));
				//cmp.body->ptr()->setAngularFactor(btVector3(0, 1, 0));
			}),
			//engine::eleInit<engine::sensorEle>()
		}, util::nextID());
		auto playerModel = e.man->create("Player Model", {
			engine::eleInit<engine::meshFilter>::run([](engine::meshFilter& filt) {
				filt.setState(1, 1, 8);
				filt.startAnimation(0, graphics::behavior::repeat);
			})
		}, util::nextID());
		playerModel.setParent(ent2);
		//playerModel.setLocalScale(vec3(0.5f, 0.4f, 0.5f));
		playerModel.setLocalPosition(glm::vec3(0.0f, -0.67f, 0.0f));
		//playerModel.setLocalOrientation(glm::rotate(-3.1415926f / 2, glm::vec3(1.0f, 0.0f, 0.0f)));
		playerModel.setLocalOrientation(glm::rotate(-3.1415926f / 2, glm::vec3(0.0f, 1.0f, 0.0f)));

		/*e.man->create("Walker Test", {
			engine::eleInit<engine::meshFilter>::run([](engine::meshFilter& filt) {
				filt.setState(1, 0, 0);
				filt.startAnimation(0, graphics::behavior::repeat);
			})
		}, util::nextID());*/


		for(int x = -6; x < 6; x++) {
			for(int z = -6; z < 6; z++) {
				e.man->create("Floor: <" + std::to_string(x) + ", " + std::to_string(z) + ">", {
					engine::eleInit<engine::rigidBodyComponent>::run([x,z](engine::rigidBodyComponent& cmp) {
						cmp.setToBox(glm::vec3(0.5f, 0.5f, 0.5f));
						cmp.ent().setLocalPosition(glm::vec3(x, -5.0f + sin(x * 0.2f) + cos(z * 0.2f), z));
						cmp.body->dynamic = false;
					}),
					engine::eleInit<engine::meshFilter>::run([x,z](engine::meshFilter& m) {
						m.setState(0, 0, 9);
						m.setNormalMap(10);
						m.visible = true;
					}),
					//engine::eleInit<engine::sensorEle>()
				}, util::nextID());
			}
		}

		e.start();

		util::spin_until([&e]() { return e.getRenderState() == engine::engine::render_doingRender; });

		bool exited = true;

		while(!e.stopped()) {
			std::string line;
			std::cout << "esh$ ";
			std::getline(std::cin, line);

			e.shell(line);
		}
	}
	
	terminateGLFW();

	util::sout("Done\n");

	return 0;
}
