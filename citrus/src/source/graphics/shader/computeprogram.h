#pragma once
#ifndef COMPUTEPROGRAM_H
#define COMPUTEPROGRAM_H

#include <string>
#include <vector>
#include <exception>

#include "graphics_h.h"
#include "storageView.h"

namespace citrus {
	namespace graphics {
		class computeprogram {
		private:
			GLuint _ptr;
			bool _good;
			std::string _log;

			void compile(std::string src) {
				GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

				{ //compile shader and store log
					const char* srcPtr = src.c_str();
					GLint length = GLint(src.length());
					glShaderSource(shader, 1, &srcPtr, &length);
					glCompileShader(shader);

					GLint status = GL_FALSE;
					glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
					_good = status == GL_TRUE;

					int logLength;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
					if (logLength > 0) {
						std::vector<char> log(logLength + 1);
						glGetShaderInfoLog(shader, logLength, NULL, log.data());
						_log = std::string(log.data()) + "~~~~ END OF SHADER LOG ~~~~\n";
					}
				}

				if (_good) {

					_log += "~~~~ START OF PROGRAM LOG ~~~~\n";

					_ptr = glCreateProgram();
					glAttachShader(_ptr, shader);
					glLinkProgram(_ptr);

					GLint status = GL_FALSE;
					glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
					_good = status == GL_TRUE;

					int logLength;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
					if (logLength > 0) {
						std::vector<char> log(logLength + 1);
						glGetShaderInfoLog(shader, logLength, NULL, log.data());
						_log += std::string(log.data());
					}
				}

				glDeleteShader(shader);
			}
		public:
			GLuint ptr() {
				return _ptr;
			}
			bool good() const {
				return _good;
			}
			std::string log() const {
				return _log;
			}

			void bindBuffer(const storageView& buf, int index) {
				glUseProgram(_ptr);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf.ptr());
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buf.ptr());
			}

			void dispatch(size_t x, size_t y = 1, size_t z = 1) {
				if (_good) {
					glUseProgram(_ptr);
					glDispatchCompute(GLuint(x), GLuint(y), GLuint(z));
				} else {
					throw std::exception("Can't dispatch a shader that isn't good");
				}
			}

			computeprogram(std::string src) {
				compile(src);
			}
			~computeprogram() {
				glDeleteProgram(_ptr);
			}
		private:

		};
	}
}

#endif