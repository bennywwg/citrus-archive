#include <citrus/util.h>

#include <citrus/graphics/instance.h>

#include <iomanip>

#include <citrus/engine/engine.h>
#include <citrus/engine/freeCam.h>
#include <citrus/engine/renderManager.h>
#include <citrus/engine/worldManager.h>
#include <citrus/engine/meshFilter.h>
#include <citrus/engine/rigidBodyComponent.h>
#include <citrus/engine/playerController.h>
#include <citrus/engine/worldManager.h>
#include <citrus/engine/rigidBodyComponent.h>
#include <citrus/engine/projectile.h>
#include <citrus/engine/sensorEle.h>

#include <citrus/engine/entityRef.inl>
#include <citrus/engine/elementRef.inl>
#include <citrus/engine/manager.inl>
#include <citrus/graphics/vkShader.h>
#include <citrus/graphics/image.h>
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
	util::sout("Citrus 0.0.0 - DO NOT DISTRIBUTE\n");

	graphics::image4b img("res/textures/grid.png");

	initializeGLFW();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	auto win = glfwCreateWindow(640, 480, "test", nullptr, nullptr);

	graphics::meshDescription desc;
	{
		graphics::mesh m;
		m.pos.push_back(vec3(0,0,0));
		m.norm.push_back(vec3(0,0,0));
		m.tangent.push_back(vec3(0,0,0));
		m.uv.push_back(vec2(0,0));
		m.bone0.push_back(0);
		m.bone1.push_back(0);
		m.weight0.push_back(0.0f);
		m.weight1.push_back(0.0f);
		desc = m.getDescription();
	}
	
	

	graphics::instance * inst = new graphics::instance("ctvk", win);
    

	graphics::mesh me("res/meshes/human.dae");
    

	graphics::model *mo = new graphics::model(*inst, me);
    

	//graphics::vkShader *sh = new graphics::vkShader(*inst, desc, {}, 640, 480,
	//	"res/shaders/standard.vert.spv",
	//	"",
	//	"res/shaders/standard.frag.spv");
	
	graphics::ctTexture tex = inst->createTexture4b(img.width(), img.height(), img.data());
    
	
	for(int i = 0; i < inst->_finalPass->targets.size(); i++) {
		auto fpfbo = inst->_finalPass->targets[i];
		inst->_finalPass->bindTexture(i, tex);
		inst->_finalPass->beginBufferAndRenderPass(fpfbo);
		inst->_finalPass->bindPipelineAndDraw(fpfbo);
		inst->_finalPass->endRenderPassAndBuffer(fpfbo);
	}
	
	
	for(int i = 0; i < 10000; i++) {
		inst->drawFrame();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cin.get();

	//delete sh;
	
	delete mo;

	delete inst;

	return 0;
	
	/*{
		engine::engine e(1.0 / 100.0);
		
		e.man->registerType<engine::renderManager>("Render Manager", true);
		e.man->registerType<engine::freeCam>("Free Cam", true);
		e.man->registerType<engine::meshManager>("Mesh Manager", false);
		e.man->registerType<engine::shaderManager>("Shader Manager", false);
		e.man->registerType<engine::meshFilter>("Mesh Filter", false);
		e.man->registerType<engine::playerController>("Player Controller", true);
		e.man->registerType<engine::worldManager>("World Manager", true);
		e.man->registerType<engine::rigidBodyComponent>("Rigid Body", false);
		e.man->registerType<engine::sensorEle>("Sensor", false);
		e.man->registerType<engine::projectile>("Projectile", true);
		e.man->setOrder({
			typeid(engine::worldManager),
			typeid(engine::sensorEle),
			typeid(engine::rigidBodyComponent),
			typeid(engine::shaderManager),
			typeid(engine::meshManager),
			typeid(engine::freeCam),
			typeid(engine::playerController),
			typeid(engine::projectile),
			typeid(engine::renderManager),
			typeid(engine::meshFilter)
		});

		std::string resDir = "/home/benny/Desktop/folder/citrus/res/";

		e.man->create("MeshTable", {
			engine::eleInit<engine::meshManager>::run(
				[resDir](engine::meshManager& man) {
					man.loadAnimation(resDir + "animations/run.cta", 0);
					man.loadAnimation(resDir + "animations/idle.cta", 1);

					man.loadMesh(resDir + "meshes/natsuki.dae", 0);
					man.loadMesh(resDir + "meshes/sphere.dae", 1);
					man.loadMesh(resDir + "meshes/cube1x1x1.dae", 2);
					man.loadMesh(resDir + "meshes/walker.dae", 3);
					man.loadMesh(resDir + "meshes/human.dae", 4);
					man.loadMesh(resDir + "meshes/blast.dae", 5);
					man.loadMesh(resDir + "meshes/icosphere.dae", 6);
					man.loadMesh(resDir + "meshes/arrow.dae", 7);

					man.bindAllAvailableAnimations();
				}
			)
			}, util::nextID());

		entityRef cam2 = e.man->create("Test 2", {
			engine::eleInit<engine::freeCam>::run(
				[](engine::freeCam& c) {
					c.cam.aspectRatio = 16.0f / 9.0f;
					c.cam.zFar = 1500.0f;
					c.cam.verticalFOV = 120.0f;
				}
			)
		}, util::nextID());

		e.man->create("Renderer", {
			engine::eleInit<engine::renderManager>::run(
				[resDir,&cam2](engine::renderManager& man) {
					man.camRef = cam2.getElement<engine::freeCam>();

					man.loadPNG(resDir + "textures/Natsuki_COLOR.png", 0);
					man.loadPNG(resDir + "textures/gridsmall.png", 1);
					man.loadPNG(resDir + "textures/cement.png", 2);
					man.loadPNG(resDir + "textures/consolas256x256.png", 3);
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
				filt.setState(4, 3, 1);
				filt.startAnimation(0, graphics::behavior::repeat);
			})
		}, util::nextID());
		playerModel.setParent(ent2);
		//playerModel.setLocalScale(vec3(0.5f, 0.4f, 0.5f));
		playerModel.setLocalPosition(glm::vec3(0.0f, -0.55f, 0.0f));
		//playerModel.setLocalOrientation(glm::rotate(-3.1415926f / 2, glm::vec3(1.0f, 0.0f, 0.0f)));
		playerModel.setLocalOrientation(glm::rotate(-3.1415926f / 2, glm::vec3(0.0f, 1.0f, 0.0f)));

		e.man->create("Walker Test", {
			engine::eleInit<engine::meshFilter>::run([](engine::meshFilter& filt) {
				filt.setState(4, 1, 1);
				filt.startAnimation(0, graphics::behavior::repeat);
			})
		}, util::nextID());

		for(int x = -1; x < 15; x++) {
			for(int z = -1; z < 15; z++) {
				e.man->create("Floor: <" + std::to_string(x) + ", " + std::to_string(z) + ">", {
					engine::eleInit<engine::rigidBodyComponent>::run([x,z](engine::rigidBodyComponent& cmp) {
						cmp.setToBox(glm::vec3(0.5f, 0.5f, 0.5f));
						cmp.ent().setLocalPosition(glm::vec3(x, -5.0f + sin(x * 0.2f) + cos(z * 0.2f), z));
						cmp.body->dynamic = false;
					}),
					engine::eleInit<engine::meshFilter>::run([x,z](engine::meshFilter& m) {
						m.setState(2, 2, 0);
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
			std::cout << "citrus $ ";
			std::getline(std::cin, line);

			if(line == "save") {
				//json savedJS = e.man->savePrefab(world);
				//std::string saved = savedJS.dump(2);
			} else if(line == "log") {
				for(auto l : e.flushLog()) {
					std::stringstream ss;
					ss << std::fixed << std::setfill('0') << std::setw(8) << std::setprecision(3) << l.first;
					util::sout(ss.str() + ": " + l.second + "\n");
				}
			} else if(line == "fps") {
				util::sout(std::to_string(e.fps()) + "\n");
			} else if(line == "obj") {
				for(auto ent : e.man->allEntities()) {
					glm::vec3 pos = ent.getGlobalTransform().getPosition();
					util::sout(ent.name() + util::toString(pos) + "\n");
					
				}
			} else if(line == "exit" || line == "stop") {
				util::sout("Halting...\n");
				e.stop();
				exited = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			} else {
				std::cout << line << "\n";
			}

		}
	}
	
	terminateGLFW();

	util::sout("Done\n");

	return 0; */







	

	/*a
	citrus::dynamics::world world;

	citrus::dynamics::collisionShape floorShape(std::vector<glm::vec3>{
		glm::vec3(10.0f, 0.0f, 0.0f),
		glm::vec3(-5.0f, 0.0f, 10.0f),
		glm::vec3(-5.0f, 1.0f, -10.0f),
		glm::vec3(0.0f, -5.0f, 1.0f),
	});

	citrus::dynamics::rigidBody staticBody(floorShape);
	staticBody.setFixed(true);
	world.addBody(staticBody);

	citrus::dynamics::collisionShape tetShape(std::vector<glm::vec3>{
		glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
	});

	citrus::dynamics::rigidBody body(tetShape);
	world.addBody(body);
	
	for (int i = 0; i < 100; i++) {
		world.step();
		std::cout << body.getPosition().y << "\n";
	}*/

	/*
	standardResourceContainer standardStuff;

	image3b *colorImage = new image3b("C:\\Users\\Benny\\Desktop\\Natsuki_COLOR.png");
	texture3b *kizuna = new texture3b(*colorImage);

	citrus::geom::conventionalmesh mesh("C:\\Users\\Benny\\Desktop\\test.dae");

	texture3b *colorTexture = new texture3b(*colorImage);

	//create shader that draws textured triangles
	program sh(
		"#version 330\n"
		"layout(location = 0) in vec3 vert_position;\n"
		"layout(location = 1) in vec3 vert_normal;\n"
		"layout(location = 2) in vec2 vert_uv;\n"
		"layout(location = 3) in uint vert_bone0;\n"
		"layout(location = 4) in uint vert_bone1;\n"
		"layout(location = 5) in float vert_weight0;\n"
		"layout(location = 6) in float vert_weight1;\n"
		""
		"uniform mat4 projectionMat;\n"
		"uniform mat4 viewMat;\n"
		"uniform mat4 modelMat;\n"
		"uniform mat4 modelViewProjectionMat;\n"
		"uniform mat4 boneData[64];\n"
		"uniform uint viewBone;\n"
		""
		"out vec2 inter_uv;\n"
		"out float inter_weight;\n"
		"void main() {\n"
		//"  vec3 transformed0 = vert_position * vert_weight0;\n"
		//"  vec3 transformed1 = vert_position * vert_weight1;\n"
		//"  if(vert_bone0 == -1) "
		//"    transformed0 = vec3(boneData[vert_bone0] * vec4(vert_position, 1.0)) * vert_weight0;\n"
		//"  }\n"
		//"  if(vert_bone1 == -1) "
		//"    transformed1 = vec3(boneData[vert_bone1] * vec4(vert_position, 1.0)) * vert_weight1;\n"
		//"  }\n"
		//"  gl_Position = modelViewProjectionMat * vec4(transformed0 + transformed1, 1.0);\n"
		"  gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);\n"
		"  inter_uv = vert_uv;\n"
		"  if(vert_bone0 == viewBone) {\n"
		"    inter_weight = vert_weight0;\n"
		"  } else {\n"
		"    inter_weight = 0.0;\n"
		"  }"
		"}\n",

		"#version 330\n"
		"in vec2 inter_uv;\n"
		"in float inter_weight;\n"
		"out vec3 color;\n"
		"uniform sampler2D tex;\n"
		"void main() {\n"
		"  color = texture(tex, inter_uv).rgb;\n"
		"  color = color + vec3(inter_weight, -inter_weight, -inter_weight);\n"
		"}\n"
	);

	program texSh(
		"#version 330\n"
		"layout(location = 0) in vec3 vert_position;\n"
		"layout(location = 2) in vec2 vert_uv;\n"
		""
		"uniform mat4 modelViewProjectionMat;\n"
		""
		"out vec2 inter_uv;\n"
		""
		"void main() {\n"
		"  gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);\n"
		"  inter_uv = vert_uv;\n"
		"}\n",

		"#version 330\n"
		"in vec2 inter_uv;\n"
		"out vec3 color;\n"
		"uniform sampler2D tex;\n"
		"uniform sampler2D texK;\n"
		"const float pixel = 1.0 / 512.0;\n"
		"void main() {\n"
		"  vec3 sample0 = texture(tex, inter_uv + vec2(pixel, 0)).rgb;\n"
		"  vec3 sample1 = texture(tex, inter_uv + vec2(-pixel, 0)).rgb;\n"
		"  vec3 sample2 = texture(tex, inter_uv + vec2(0, pixel)).rgb;\n"
		"  vec3 sample3 = texture(tex, inter_uv + vec2(0, -pixel)).rgb;\n"
		"  color = (sample0 + sample1 + sample2 + sample3) * 0.25;\n"
		//"  color = (texture(tex, inter_uv).rgb + texture(texK, inter_uv).rgb) * 0.5;\n"
		"}\n"
	);

	if (!texSh.good()) std::cout << texSh.log();

	program pointSh(
		"#version 330\n"
		"layout(location = 0) in vec3 vert_position;\n"
		""
		"uniform mat4 modelViewProjectionMat;\n"
		"void main() {\n"
		"  gl_Position = modelViewProjectionMat * vec4(vert_position, 1.0);\n"
		"}\n",

		"#version 330\n"
		"out vec3 color;\n"
		"void main() {\n"
		"  color = vec3(1,0,0);\n"
		"}\n"
	);


	computeprogram csh(
		"#version 330\n"
		"layout(std430, binding = 0) buffer vertData {\n"
		"  vec4 positions[];\n"
		"};\n"
		"layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;"
		"void main() {\n"
		"  positions[gl_GlobalInvocationID.x].x = positions[gl_GlobalInvocationID.x].x + 0.001f;\n"
		"}\n"
	);

	std::shared_ptr<buffer> einGrosBuf(new buffer(15 * 1024 * 1024));
	citrus::geom::riggedModel model(mesh, einGrosBuf);
	vertexArray vao({
		vertexAttribute(0, false, *model.vertices),
		vertexAttribute(2, false, *model.uvs),
		vertexAttribute(3, false, *model.bones0),
		vertexAttribute(4, false, *model.bones1),
		vertexAttribute(5, false, *model.weights0),
		vertexAttribute(6, false, *model.weights1),
	}, *model.indices);


	//bones
	std::shared_ptr<buffer> smallBuff(new buffer(1 * 1024 * 1024));
	std::vector<glm::vec3> boneVertsVec;
	std::vector<unsigned int> boneIndicesVec;
	std::vector<citrus::geom::boneNode> demBones;
	for (int i = 0; i < demBones.size(); i++) {
		glm::mat4 mat = demBones[i].accumulatedTransform;
		boneVertsVec.push_back(-glm::vec3(mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) / 100.0f);
		boneVertsVec.push_back(boneVertsVec[boneVertsVec.size() - 1] + glm::vec3(mat * glm::vec4(0.0f, 0.01f, 0.0f, 0.0f)));
		boneIndicesVec.push_back(i * 2);
		//boneIndicesVec.push_back(i * 2 + 1);
	}
	std::shared_ptr<vertexView3f> boneVertsView = std::make_shared<vertexView3f>(smallBuff, 0, boneVertsVec);
	std::shared_ptr<indexViewui> boneIndicesView = std::make_shared<indexViewui>(smallBuff, 256 * 1024, boneIndicesVec);
	vertexArray bonePointsVAO({
		vertexAttribute(0, false, *boneVertsView)
	}, *boneIndicesView);
	
	//set up regular framebuffer (depth buffer + 1 color buffer)
	depthTexture24 depthTex(512, 512);
	texture3b colorTex(image3b(512, 512));
	std::vector<colorAttachment> attachments({
		colorAttachment(&colorTex, 0)
	});
	frameBuffer fbo(attachments, &depthTex, 512, 512);

	//get a handle to the output framebuffer
	frameBuffer screen(win);
	screen.bind();

	while (!win->shouldClose()) {
		if (bone >= mesh.bones.bones.size()) bone = 0;


		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glPointSize(4.0f);


		glfwGetTime();

		pointSh.setUniform("modelViewProjectionMat",
			glm::translate(glm::vec3(0.0f, 0.0f, 0.0f))
			//glm::scale(glm::vec3(0.8f, 0.8f, 0.8f)) *
			//glm::rotate(glm::radians(float(glfwGetTime()) * 10.0f), glm::vec3(0.0f, 1.0f, 0.0f))
			//glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		);
		sh.setUniform("modelViewProjectionMat", 
			glm::rotate(glm::radians(float(glfwGetTime()) * 40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

		glm::ivec2 fbsize = win->framebufferSize();

		fbo.clearAll();
		fbo.bind();

		sh.use();
		sh.setUniform("viewBone", bone);
		kizuna->bind(GL_TEXTURE0);
		sh.setUniform("tex", 0);
		vao.drawAll();


		screen.clearAll();
		screen.bind();
		
		texSh.use();
		kizuna->bind(GL_TEXTURE0);
		texSh.setUniform("texK", 0);
		fbo.getColors()[0].tex->bind(GL_TEXTURE1);
		texSh.setUniform("tex", 1);
		texSh.setUniform("modelViewProjectionMat", glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));
		standardStuff.quadVAO.drawAll();

		//glDisable(GL_DEPTH_TEST);
		//glUseProgram(pointSh.ptr());
		//bonePointsVAO.drawAll(GL_POINTS);


		win->swapBuffers();
		glfwPollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}

	


	return 0;*/
}
