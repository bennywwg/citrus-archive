#pragma once

#ifndef TYPEENUMS_H
#define TYPEENUMS_H

#include <glm/ext.hpp>

#include "GL/glew.h"

namespace citrus {
	namespace graphics {

		enum glslType {
			glsl_ubyte, glsl_byte, glsl_ushort, glsl_short, glsl_uint, glsl_int, glsl_float, glsl_double,
			glsl_ubvec2, glsl_bvec2, glsl_usvec2, glsl_svec2, glsl_uvec2, glsl_ivec2, glsl_vec2, glsl_dvec2,
			glsl_ubvec3, glsl_bvec3, glsl_usvec3, glsl_svec3, glsl_uvec3, glsl_ivec3, glsl_vec3, glsl_dvec3,
			glsl_ubvec4, glsl_bvec4, glsl_usvec4, glsl_svec4, glsl_uvec4, glsl_ivec4, glsl_vec4, glsl_dvec4,
			glsl_mat2, glsl_dmat2,
			glsl_mat3, glsl_dmat3,
			glsl_mat4, glsl_dmat4
		};

		template<typename T> struct glslTypeTraits {};

		template<> struct glslTypeTraits<unsigned char> { static const glslType type = glsl_ubyte; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<char> { static const glslType type = glsl_byte; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<unsigned short> { static const glslType type = glsl_ushort; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<short> { static const glslType type = glsl_short; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<unsigned int> { static const glslType type = glsl_uint; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<int> { static const glslType type = glsl_int; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<float> { static const glslType type = glsl_float; static const unsigned int components = 1; };
		template<> struct glslTypeTraits<double> { static const glslType type = glsl_double; static const unsigned int components = 1; };

		template<> struct glslTypeTraits<glm::tvec2<unsigned char>> { static const glslType type = glsl_ubvec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::tvec2<char>> { static const glslType type = glsl_bvec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::tvec2<unsigned short>> { static const glslType type = glsl_usvec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::tvec2<short>> { static const glslType type = glsl_svec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::uvec2> { static const glslType type = glsl_uvec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::ivec2> { static const glslType type = glsl_ivec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::vec2> { static const glslType type = glsl_vec2; static const unsigned int components = 2; };
		template<> struct glslTypeTraits<glm::dvec2> { static const glslType type = glsl_dvec2; static const unsigned int components = 2; };

		template<> struct glslTypeTraits<glm::tvec3<unsigned char>> { static const glslType type = glsl_ubvec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::tvec3<char>> { static const glslType type = glsl_bvec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::tvec3<unsigned short>> { static const glslType type = glsl_usvec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::tvec3<short>> { static const glslType type = glsl_svec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::uvec3> { static const glslType type = glsl_uvec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::ivec3> { static const glslType type = glsl_ivec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::vec3> { static const glslType type = glsl_vec3; static const unsigned int components = 3; };
		template<> struct glslTypeTraits<glm::dvec3> { static const glslType type = glsl_dvec3; static const unsigned int components = 3; };

		template<> struct glslTypeTraits<glm::tvec4<unsigned char>> { static const glslType type = glsl_ubvec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::tvec4<char>> { static const glslType type = glsl_bvec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::tvec4<unsigned short>> { static const glslType type = glsl_usvec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::tvec4<short>> { static const glslType type = glsl_svec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::uvec4> { static const glslType type = glsl_uvec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::ivec4> { static const glslType type = glsl_ivec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::vec4> { static const glslType type = glsl_vec4; static const unsigned int components = 4; };
		template<> struct glslTypeTraits<glm::dvec4> { static const glslType type = glsl_dvec4; static const unsigned int components = 4; };

		template<> struct glslTypeTraits<glm::mat2> { static const glslType type = glsl_mat2; };
		template<> struct glslTypeTraits<glm::dmat2> { static const glslType type = glsl_dmat2; };

		template<> struct glslTypeTraits<glm::mat3> { static const glslType type = glsl_mat3; };
		template<> struct glslTypeTraits<glm::dmat3> { static const glslType type = glsl_dmat3; };

		template<> struct glslTypeTraits<glm::mat4> { static const glslType type = glsl_mat4; };
		template<> struct glslTypeTraits<glm::dmat4> { static const glslType type = glsl_dmat4; };

		template<typename T> struct isStorageType { static const bool value = false; };
		template<> struct isStorageType<unsigned int> { static const bool value = true; };
		template<> struct isStorageType<int> { static const bool value = true; };
		template<> struct isStorageType<float> { static const bool value = true; };
		template<> struct isStorageType<double> { static const bool value = true; };
		template<> struct isStorageType<glm::uvec2> { static const bool value = true; };
		template<> struct isStorageType<glm::ivec2> { static const bool value = true; };
		template<> struct isStorageType<glm::vec2> { static const bool value = true; };
		template<> struct isStorageType<glm::dvec2> { static const bool value = true; };
		template<> struct isStorageType<glm::uvec3> { static const bool value = true; };
		template<> struct isStorageType<glm::ivec3> { static const bool value = true; };
		template<> struct isStorageType<glm::vec3> { static const bool value = true; };
		template<> struct isStorageType<glm::dvec3> { static const bool value = true; };
		template<> struct isStorageType<glm::uvec4> { static const bool value = true; };
		template<> struct isStorageType<glm::ivec4> { static const bool value = true; };
		template<> struct isStorageType<glm::vec4> { static const bool value = true; };
		template<> struct isStorageType<glm::dvec4> { static const bool value = true; };
		template<> struct isStorageType<glm::mat2> { static const bool value = true; };
		template<> struct isStorageType<glm::dmat2> { static const bool value = true; };
		template<> struct isStorageType<glm::mat3> { static const bool value = true; };
		template<> struct isStorageType<glm::dmat3> { static const bool value = true; };
		template<> struct isStorageType<glm::mat4> { static const bool value = true; };
		template<> struct isStorageType<glm::dmat4> { static const bool value = true; };

		template<typename T> struct isVertexType { static const bool value = false; };
		template<> struct isVertexType<unsigned char> { static const bool value = true; };
		template<> struct isVertexType<char> { static const bool value = true; };
		template<> struct isVertexType<unsigned short> { static const bool value = true; };
		template<> struct isVertexType<short> { static const bool value = true; };
		template<> struct isVertexType<unsigned int> { static const bool value = true; };
		template<> struct isVertexType<int> { static const bool value = true; };
		template<> struct isVertexType<float> { static const bool value = true; };
		template<> struct isVertexType<double> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec2<unsigned char>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec2<char>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec2<unsigned short>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec2<short>> { static const bool value = true; };
		template<> struct isVertexType<glm::uvec2> { static const bool value = true; };
		template<> struct isVertexType<glm::ivec2> { static const bool value = true; };
		template<> struct isVertexType<glm::vec2> { static const bool value = true; };
		template<> struct isVertexType<glm::dvec2> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec3<unsigned char>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec3<char>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec3<unsigned short>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec3<short>> { static const bool value = true; };
		template<> struct isVertexType<glm::uvec3> { static const bool value = true; };
		template<> struct isVertexType<glm::ivec3> { static const bool value = true; };
		template<> struct isVertexType<glm::vec3> { static const bool value = true; };
		template<> struct isVertexType<glm::dvec3> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec4<unsigned char>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec4<char>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec4<unsigned short>> { static const bool value = true; };
		template<> struct isVertexType<glm::tvec4<short>> { static const bool value = true; };
		template<> struct isVertexType<glm::uvec4> { static const bool value = true; };
		template<> struct isVertexType<glm::ivec4> { static const bool value = true; };
		template<> struct isVertexType<glm::vec4> { static const bool value = true; };
		template<> struct isVertexType<glm::dvec4> { static const bool value = true; };

		template<typename T> struct isIndexType { static const bool value = false; };
		template<> struct isIndexType<unsigned char> { static const bool value = true; };
		template<> struct isIndexType<unsigned short> { static const bool value = true; };
		template<> struct isIndexType<unsigned int> { static const bool value = true; };

		inline size_t glslTypeSize(glslType type) {
			if (type == glsl_ubyte || type == glsl_byte) {
				return 1;
			} else if (type == glsl_ushort || type == glsl_short) {
				return 2;
			} else if (type == glsl_uint || type == glsl_int || type == glsl_float) {
				return 4;
			} else if (type == glsl_double) {
				return 8;
			} else if (type == glsl_ubvec2 || type == glsl_bvec2) {
				return 2;
			} else if (type == glsl_usvec2 || type == glsl_svec2) {
				return 4;
			} else if (type == glsl_uvec2 || type == glsl_ivec2 || type == glsl_vec2) {
				return 8;
			} else if (type == glsl_dvec2) {
				return 16;
			} else if (type == glsl_ubvec3 || type == glsl_bvec3) {
				return 3;
			} else if (type == glsl_usvec3 || type == glsl_svec3) {
				return 6;
			} else if (type == glsl_uvec3 || type == glsl_ivec3 || type == glsl_vec3) {
				return 12;
			} else if (type == glsl_dvec3) {
				return 24;
			} else if (type == glsl_ubvec4 || type == glsl_bvec4) {
				return 4;
			} else if (type == glsl_usvec4 || type == glsl_svec4) {
				return 8;
			} else if (type == glsl_uvec4 || type == glsl_ivec4 || type == glsl_vec4) {
				return 16;
			} else if (type == glsl_dvec4) {
				return 32;
			} else if (type == glsl_mat2) {
				return 16;
			} else if (type == glsl_dmat2) {
				return 32;
			} else if (type == glsl_mat3) {
				return 36;
			} else if (type == glsl_dmat3) {
				return 72;
			} else if (type == glsl_mat4) {
				return 64;
			} else if (type == glsl_dmat4) {
				return 128;
			} else {
				return 0;
			}
		}

		inline unsigned int glslTypeComponents(glslType type) {
			if (type == glsl_ubyte || type == glsl_byte || type == glsl_ushort || type == glsl_short || type == glsl_uint || type == glsl_int || type == glsl_float || type == glsl_double) {
				return 1;
			} else if (type == glsl_ubvec2 || type == glsl_bvec2 || type == glsl_usvec2 || type == glsl_svec2 || type == glsl_uvec2 || type == glsl_ivec2 || type == glsl_vec2 || type == glsl_dvec2) {
				return 2;
			} else if (type == glsl_ubvec3 || type == glsl_bvec3 || type == glsl_usvec3 || type == glsl_svec3 || type == glsl_uvec3 || type == glsl_ivec3 || type == glsl_vec3 || type == glsl_dvec3) {
				return 3;
			} else if (type == glsl_ubvec4 || type == glsl_bvec4 || type == glsl_usvec4 || type == glsl_svec4 || type == glsl_uvec4 || type == glsl_ivec4 || type == glsl_vec4 || type == glsl_dvec4) {
				return 4;
			} else {
				return 0;
			}
		}

		inline GLenum gpuGLType(glslType type) {
			if (type == glsl_ubyte || type == glsl_ubvec2 || type == glsl_ubvec3 || type == glsl_ubvec4)
				return GL_UNSIGNED_BYTE;
			else if (type == glsl_byte || type == glsl_bvec2 || type == glsl_bvec3 || type == glsl_bvec4)
				return GL_BYTE;
			else if (type == glsl_ushort || type == glsl_usvec2 || type == glsl_usvec3 || type == glsl_usvec4)
				return GL_UNSIGNED_SHORT;
			else if (type == glsl_short || type == glsl_svec2 || type == glsl_svec3 || type == glsl_svec4)
				return GL_SHORT;
			else if (type == glsl_uint || type == glsl_uvec2 || type == glsl_uvec3 || type == glsl_uvec4)
				return GL_UNSIGNED_INT;
			else if (type == glsl_int || type == glsl_ivec2 || type == glsl_ivec3 || type == glsl_ivec4)
				return GL_INT;
			else if (type == glsl_float || type == glsl_vec2 || type == glsl_vec3 || type == glsl_vec4)
				return GL_FLOAT;
			else
				return GL_DOUBLE;
		}

		template<GLint glType> struct textureInternalFormatTraits {
			static const bool isFormat = false; //is this GLint a valid internal texture format?
			static const bool isColor = false; //is this GLint a valid internal texture format for color?
			static const bool isDepth = false; //is this GLint a valid internal texture format for depth?
		};

		template<> struct textureInternalFormatTraits<GL_R8> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R8_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R16> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R16_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG8> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG8_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG16> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG16_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R3_G3_B2> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB4> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB5> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB8> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB8_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB10> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB12> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB16_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA2> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA4> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB5_A1> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA8> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA8_SNORM> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB10_A2> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB10_A2UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA12> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA16> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_SRGB8> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_SRGB8_ALPHA8> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R16F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG16F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB16F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA16F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R32F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG32F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB32F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA32F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R11F_G11F_B10F> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB9_E5> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R8I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R8UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R16I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R16UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R32I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_R32UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG8I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG8UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG16I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG16UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG32I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RG32UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB8I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB8UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB16I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB16UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB32I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGB32UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA8I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA8UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA16I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA16UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA32I> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_RGBA32UI> { static const bool isFormat = true; static const bool isColor = true; static const bool isDepth = false; };
		template<> struct textureInternalFormatTraits<GL_DEPTH_COMPONENT16> { static const bool isFormat = true; static const bool isColor = false; static const bool isDepth = true; };
		template<> struct textureInternalFormatTraits<GL_DEPTH_COMPONENT24> { static const bool isFormat = true; static const bool isColor = false; static const bool isDepth = true; };
		template<> struct textureInternalFormatTraits<GL_DEPTH_COMPONENT32> { static const bool isFormat = true; static const bool isColor = false; static const bool isDepth = true; };
		template<> struct textureInternalFormatTraits<GL_DEPTH_COMPONENT32F> { static const bool isFormat = true; static const bool isColor = false; static const bool isDepth = true; };

		inline bool isInternalFormat(GLint code) {
			return
				GL_R8 == code ||
				GL_R8_SNORM == code ||
				GL_R16 == code ||
				GL_R16_SNORM == code ||
				GL_RG8 == code ||
				GL_RG8_SNORM == code ||
				GL_RG16 == code ||
				GL_RG16_SNORM == code ||
				GL_R3_G3_B2 == code ||
				GL_RGB4 == code ||
				GL_RGB5 == code ||
				GL_RGB8 == code ||
				GL_RGB8_SNORM == code ||
				GL_RGB10 == code ||
				GL_RGB12 == code ||
				GL_RGB16_SNORM == code ||
				GL_RGBA2 == code ||
				GL_RGBA4 == code ||
				GL_RGB5_A1 == code ||
				GL_RGBA8 == code ||
				GL_RGBA8_SNORM == code ||
				GL_RGB10_A2 == code ||
				GL_RGB10_A2UI == code ||
				GL_RGBA12 == code ||
				GL_RGBA16 == code ||
				GL_SRGB8 == code ||
				GL_SRGB8_ALPHA8 == code ||
				GL_R16F == code ||
				GL_RG16F == code ||
				GL_RGB16F == code ||
				GL_RGBA16F == code ||
				GL_R32F == code ||
				GL_RG32F == code ||
				GL_RGB32F == code ||
				GL_RGBA32F == code ||
				GL_R11F_G11F_B10F == code ||
				GL_RGB9_E5 == code ||
				GL_R8I == code ||
				GL_R8UI == code ||
				GL_R16I == code ||
				GL_R16UI == code ||
				GL_R32I == code ||
				GL_R32UI == code ||
				GL_RG8I == code ||
				GL_RG8UI == code ||
				GL_RG16I == code ||
				GL_RG16UI == code ||
				GL_RG32I == code ||
				GL_RG32UI == code ||
				GL_RGB8I == code ||
				GL_RGB8UI == code ||
				GL_RGB16I == code ||
				GL_RGB16UI == code ||
				GL_RGB32I == code ||
				GL_RGB32UI == code ||
				GL_RGBA8I == code ||
				GL_RGBA8UI == code ||
				GL_RGBA16I == code ||
				GL_RGBA16UI == code ||
				GL_RGBA32I == code ||
				GL_RGBA32UI == code ||
				GL_DEPTH_COMPONENT16 == code ||
				GL_DEPTH_COMPONENT24 == code ||
				GL_DEPTH_COMPONENT32 == code ||
				GL_DEPTH_COMPONENT32F == code;
		}

		template<GLint format> struct textureInputFormatTraits { static const bool isFormat = false; };
		template<> struct textureInputFormatTraits<GL_RED> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RG> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RGB> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_BGR> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RGBA> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_BGRA> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RED_INTEGER> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RG_INTEGER> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RGB_INTEGER> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_BGR_INTEGER> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_RGBA_INTEGER> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_STENCIL_INDEX> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_DEPTH_COMPONENT> { static const bool isFormat = true; };
		template<> struct textureInputFormatTraits<GL_DEPTH_STENCIL> { static const bool isFormat = true; };

		inline bool isInputFormat(GLint code) {
			return
				code == GL_RED ||
				code == GL_RG ||
				code == GL_RGB ||
				code == GL_BGR ||
				code == GL_RGBA ||
				code == GL_BGRA ||
				code == GL_RED_INTEGER ||
				code == GL_RG_INTEGER ||
				code == GL_RGB_INTEGER ||
				code == GL_BGR_INTEGER ||
				code == GL_RGBA_INTEGER ||
				code == GL_BGRA_INTEGER ||
				code == GL_STENCIL_INDEX ||
				code == GL_DEPTH_COMPONENT ||
				code == GL_DEPTH_STENCIL;
		}

		template<GLint type> struct textureInputTypeTraits { static const bool isType = false; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_BYTE				> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_BYTE						> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT				> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_SHORT						> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_INT				> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_INT							> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_FLOAT						> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_BYTE_3_3_2			> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_BYTE_2_3_3_REV		> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT_5_6_5		> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT_5_6_5_REV	> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT_4_4_4_4		> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT_4_4_4_4_REV	> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT_5_5_5_1		> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_SHORT_1_5_5_5_REV	> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_INT_8_8_8_8		> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_INT_8_8_8_8_REV	> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_INT_10_10_10_2		> { static const bool isType = true; };
		template<> struct textureInputTypeTraits<GL_UNSIGNED_INT_2_10_10_10_REV	> { static const bool isType = true; };

		inline bool isInputType(GLint type) {
			return
				type == GL_UNSIGNED_BYTE ||
				type == GL_BYTE ||
				type == GL_UNSIGNED_SHORT ||
				type == GL_SHORT ||
				type == GL_UNSIGNED_INT ||
				type == GL_INT ||
				type == GL_FLOAT ||
				type == GL_UNSIGNED_BYTE_3_3_2 ||
				type == GL_UNSIGNED_BYTE_2_3_3_REV ||
				type == GL_UNSIGNED_SHORT_5_6_5 ||
				type == GL_UNSIGNED_SHORT_5_6_5_REV ||
				type == GL_UNSIGNED_SHORT_4_4_4_4 ||
				type == GL_UNSIGNED_SHORT_4_4_4_4_REV ||
				type == GL_UNSIGNED_SHORT_5_5_5_1 ||
				type == GL_UNSIGNED_SHORT_1_5_5_5_REV ||
				type == GL_UNSIGNED_INT_8_8_8_8 ||
				type == GL_UNSIGNED_INT_8_8_8_8_REV ||
				type == GL_UNSIGNED_INT_10_10_10_2 ||
				type == GL_UNSIGNED_INT_2_10_10_10_REV;
		}

		template<typename T> struct languageTypeTraits {};
		template<> struct languageTypeTraits<unsigned char> { static const GLint glType = GL_UNSIGNED_BYTE; };
		template<> struct languageTypeTraits<char> { static const GLint glType = GL_BYTE; };
		template<> struct languageTypeTraits<unsigned short> { static const GLint glType = GL_UNSIGNED_SHORT; };
		template<> struct languageTypeTraits<short> { static const GLint glType = GL_SHORT; };
		template<> struct languageTypeTraits<unsigned int> { static const GLint glType = GL_UNSIGNED_INT; };
		template<> struct languageTypeTraits<int> { static const GLint glType = GL_INT; };
		template<> struct languageTypeTraits<float> { static const GLint glType = GL_FLOAT; };
		template<> struct languageTypeTraits<double> { static const GLint glType = GL_DOUBLE; };
	}
}

#endif
