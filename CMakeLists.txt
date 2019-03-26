cmake_minimum_required (VERSION 3.14) #π
project (citrus)
set (citrus_VERSION 0)

set (BT_LIB_DIR "" CACHE PATH "path to bullet3 lib directory")
set (BT_INCLUDE_DIR "" CACHE PATH "path to bullet3 include directory")
set (VK_INCLUDE_DIR "" CACHE PATH "path to vulkan sdk include directory")
set (GLM_INCLUDE_DIR "" CACHE PATH "path to glm include directory")
set (GLFW_LIB_DIR "" CACHE PATH "path to glfw3 lib directory")
set (GLFW_INCLUDE_DIR "" CACHE PATH "path to glfw3 include directory")
set (ASSIMP_LIB_DIR "" CACHE PATH "path to assimp lib directory")
set (ASSIMP_INCLUDE_DIR "" CACHE PATH "path to assimp include directory")
set (ASSIMPBUILD_INCLUDE_DIR "" CACHE PATH "path to assimp include directory")
set (NFD_LIB_DIR "" CACHE PATH "path to nativefiledialog lib directory")
set (NFD_INCLUDE_DIR "" CACHE PATH "path to nativefiledialog include directory")
set (JSON_INCLUDE_DIR "" CACHE PATH "path to nlohmann json include directory")
set (PNG_INCLUDE_DIR "" CACHE PATH "path to libpng include directory")
set (PNG_LIB_DIR "" CACHE PATH "path to libpng lib directory")
set (PNGBUILD_INCLUDE_DIR "" CACHE PATH "path to libpng include directory")
set (ZLIB_LIB_DIR "" CACHE PATH "path to zlib lib directory")
set (VULKAN_LIB_DIR "" CACHE PATH "path to vulkan lib directory")

configure_file (
	"${PROJECT_SOURCE_DIR}/citrus/citrus_config.h.in"
	"${PROJECT_BINARY_DIR}/citrus/citrus_config.h"
)

include_directories (
	${PROJECT_SOURCE_DIR}
	${BT_INCLUDE_DIR}
	${VK_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${ASSIMPBUILD_INCLUDE_DIR}
	${NFD_INCLUDE_DIR}
	${JSON_INCLUDE_DIR}
	${PNG_INCLUDE_DIR}
	${PNGBUILD_INCLUDE_DIR}
)

link_directories (
	${BT_LIB_DIR}
	${GLFW_LIB_DIR}
	${NFD_LIB_DIR}
	${ASSIMP_LIB_DIR}
	${PNG_LIB_DIR}
	${ZLIB_LIB_DIR}
	${VULKAN_LIB_DIR}
)

add_definitions (-DBT_USE_DOUBLE_PRECISION)

file (GLOB_RECURSE SRC_FILES *.cpp)
file (GLOB_RECURSE HEADER_FILES *.h)

add_executable (ctvk ${SRC_FILES} ${HEADER_FILES})

target_link_libraries (ctvk zlibd glfw3dll nfd_d LinearMath_Debug BulletDynamics_Debug BulletCollision_Debug vulkan-1 assimp-vc140-mt libpng16d)

#filesystem, optional, fully qualified namespaces, etc
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set_property (TARGET ctvk PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})