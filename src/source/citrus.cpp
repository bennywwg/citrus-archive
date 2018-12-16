#include "util/util.h"

#include <iomanip>

#include "engine/engine.h"
#include "engine/elements/freeCam.h"
#include "engine/elements/renderManager.h"
#include <engine/elements/worldManager.h>
#include <engine/elements/meshManager.h>
#include <engine/elements/shaderManager.h>
#include <engine/elements/textureManager.h>
#include <engine/elements/meshFilter.h>
#include <engine/elements/rigidBodyComponent.h>
#include <engine/elements/playerController.h>
#include <engine/elements/worldManager.h>
#include <engine/elements/rigidBodyComponent.h>

#include <engine/entityRef.inl>
#include <engine/elementRef.inl>
#include <engine/manager.inl>
using citrus::engine::entityRef;

#include <GLFW/glfw3.h>

#include <sstream>

using namespace citrus;

void initializeGLFW() {
	if (!glfwInit()) throw std::runtime_error("Unable to create OpenGL context");
}
void terminateGLFW() {
	glfwTerminate();
}

int main(int argc, char **argv) {

	util::sout("Citrus 0.0.0 - PRIVATE DEVELOPMENT BUILD - DO NOT DISTRIBUTE\n");

	initializeGLFW();
	
	{
		util::scopedProfiler* prof = new util::scopedProfiler("Initializing Engine");
		engine::engine e(1.0 / 100.0);
		
		//e.man->registerType<engine::worldManager>("World Manager");
		//e.man->registerType<engine::rigidBodyComponent>("Rigid Body");
		e.man->registerType<engine::renderManager>("Render Manager");
		e.man->registerType<engine::freeCam>("Free Cam");
		e.man->registerType<engine::meshManager>("Mesh Manager");
		e.man->registerType<engine::shaderManager>("Shader Manager");
		e.man->registerType<engine::textureManager>("Texture Manager");
		e.man->registerType<engine::meshFilter>("Mesh Filter");
		e.man->registerType<engine::playerController>("Player Controller");
		e.man->registerType<engine::worldManager>("World Manager");
		e.man->registerType<engine::rigidBodyComponent>("Rigid Body");
		e.man->setOrder({
			//typeid(engine::worldManager),
			//typeid(engine::rigidBodyComponent),
			typeid(engine::worldManager),
			typeid(engine::rigidBodyComponent),
			typeid(engine::shaderManager),
			typeid(engine::meshManager),
			typeid(engine::textureManager),
			typeid(engine::freeCam),
			typeid(engine::playerController),
			typeid(engine::renderManager),
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


		e.man->create("Renderer", {
			engine::eleInit<engine::renderManager>::run(
				[&cam2](engine::renderManager& man) {
					man.camRef = cam2.getElement<engine::freeCam>();
				}
			)
		}, util::nextID());

		e.man->create("MeshTable", {
			engine::eleInit<engine::meshManager>::run(
				[](engine::meshManager& man) {
					man.loadMesh("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\meshes\\natsuki.dae", 0);
				}
			)
		}, util::nextID());
		e.man->create("TextureTable", {
			engine::eleInit<engine::textureManager>::run(
				[](engine::textureManager& man) {
					man.loadPNG("C:\\Users\\benny\\OneDrive\\Desktop\\folder\\citrus\\res\\textures\\Natsuki_COLOR.png", 0);
				}
			)
		}, util::nextID());

		e.man->create("Physics", {engine::eleInit<engine::worldManager>()}, util::nextID());

		auto ent2 = e.man->create("Player", {
			engine::eleInit<engine::playerController>(),
			engine::eleInit<engine::rigidBodyComponent>::run(
				[](engine::rigidBodyComponent& rb) {
					rb.body->dynamic = false;
				}
			)
		}, util::nextID());
		

		auto playerModel = e.man->create("Player Model", {
			engine::eleInit<engine::meshFilter>::run(
				[](engine::meshFilter& filt) {
				filt.setState(0, 0, 0);
			}
		)}, util::nextID());
		playerModel.setParent(ent2);
		playerModel.setLocalOrientation(glm::rotate(-3.1415926f / 2, glm::vec3(1.0f, 0.0f, 0.0f)));


		/*auto world = e.man->create("World", {engine::eleInit<engine::worldManager>({})}, util::nextID());
		e.man->create("Object", {
			engine::eleInit<engine::rigidBodyComponent>(nlohmann::json({
				{"world", e.man->referenceElement<engine::worldManager>(world)}
			}))
		}, util::nextID());*/

		e.start();

		util::spin_until([&e]() { return e.getRenderState() == engine::engine::render_doingRender; });
			
		delete prof; prof = nullptr;



		//e.man->loadPrefabUnsafe(savedJS);

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

	return 0;








	

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
