//#include "modelLoader.h"
//#include "mesh.h"
//
//#include <filesystem>
//
//namespace citrus {
//	namespace engine {
//		namespace fs = std::experimental::filesystem;
//
//		resourceLoader::status modelLoader::processResource(engine * e, string resourcesPath, string myPath, nlohmann::json parsed) {
//			try {
//				string path = parsed["meshPath"].get<string>();
//				string name = parsed["name"].get<string>();
//
//				geom::simpleMesh mesh((fs::path(resourcesPath) / fs::path(path)).string());
//
//				e->models->insert(map<string, geom::simpleModel*>::value_type(name, new geom::simpleModel(mesh, std::make_shared<graphics::buffer>(mesh.requiredSize()))));
//			} catch(std::exception ex) {
//				return resourceLoader::status(false, "");
//			}
//			return resourceLoader::status(true, "");
//		}
//		modelLoader::modelLoader() : resourceLoader("model", 0) { }
//	}
//}
