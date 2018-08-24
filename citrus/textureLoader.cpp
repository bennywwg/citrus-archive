//#include "textureLoader.h"
//#include "colorTexture.h"
//#include "engine.h"
//
//#include <filesystem>
//
//namespace citrus {
//	namespace engine {
//		namespace fs = std::experimental::filesystem;
//
//		resourceLoader::status textureLoader::processResource(engine * e, string resourcesPath, string myPath, nlohmann::json parsed) {
//			try {
//				string path = parsed["path"].get<string>();
//				string name = parsed["name"].get<string>();
//
//				e->textures->insert(
//					map<string, 
//					graphics::texture*>::value_type(name, new graphics::texture3b(graphics::image3b(
//						(fs::path(resourcesPath) / fs::path(path)).string()))));
//			} catch(std::exception ex) {
//				return resourceLoader::status(false, "");
//			}
//			return resourceLoader::status(true, "");
//		}
//
//		textureLoader::textureLoader() : resourceLoader("Texture", 0) { }
//
//	}
//}
