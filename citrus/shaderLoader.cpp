//#include <filesystem>
//
//#include "stdUtil.h"
//
//#include "shaderLoader.h"
//#include "shader.h"
//#include "engine.h"
//
//namespace citrus {
//	namespace engine {
//		namespace fs = std::experimental::filesystem;
//
//		resourceLoader::status shaderLoader::processResource(engine * e, string resourcesPath, string myPath, nlohmann::json parsed) {
//			try {
//				string vertPath = parsed["vertPath"].get<string>();
//				string fragPath = parsed["fragPath"].get<string>();
//				string name = parsed["name"].get<string>();
//
//				string fullVertPath = (fs::path(resourcesPath) / fs::path(vertPath)).string();
//				string fullFragPath = (fs::path(resourcesPath) / fs::path(fragPath)).string();
//
//				string vertSrc = util::loadEntireFile(fullVertPath);
//				string fragSrc = util::loadEntireFile(fullFragPath);
//
//				e->shaders->insert(map<string, graphics::shader*>::value_type(name, new graphics::shader(vertSrc, fragSrc)));
//			} catch(std::exception ex) {
//				return resourceLoader::status(false, "");
//			}
//			return resourceLoader::status(true, "");
//		}
//		shaderLoader::shaderLoader() : resourceLoader("shader", 0) { }
//	}
//}