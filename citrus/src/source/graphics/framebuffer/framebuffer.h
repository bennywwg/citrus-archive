#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>

#include <util/graphics_h.h>
#include <graphics/texture/colorTexture.h>
#include <graphics/texture/depthTexture.h>
#include <graphics/window/window.h>

namespace citrus {
	namespace graphics {

		struct colorAttachment {
			colorTexture* tex;
			unsigned int attachment;
			colorAttachment() = default;
			colorAttachment(colorTexture* tex, unsigned int attachment) : tex(tex), attachment(attachment) {}
		};

		class frameBuffer {
		private:
			GLuint _ptr;

			const unsigned int _width, _height;

			depthTexture* _depth;
			std::vector<colorAttachment> _colors;
		public:
			GLuint ptr() const {
				return _ptr;
			}

			void bind() const {
				glBindFramebuffer(GL_FRAMEBUFFER, _ptr);
			}
			void setViewport(GLint x, GLint y, GLsizei width, GLsizei height) const {
				bind();
				glViewport(x, y, width, height);
			}
			void clearColor(float r, float g, float b, float a) const {
				bind();
				glClearColor(r, g, b, a);
				glClear(GL_COLOR_BUFFER_BIT);
			}
			void clearColor() const {
				clearColor(0.0f, 0.0f, 0.0f, 1.0f);
			}
			void clearDepth() const {
				bind();
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			void clearAll(float r, float g, float b, float a) const {
				bind();
				glClearColor(r, g, b, a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			void clearAll() const {
				clearAll(0.0f, 0.0f, 0.0f, 0.0f);
			}
			unsigned int width() const {
				return _width;
			}
			unsigned int height() const {
				return _height;
			}

			depthTexture* getDepth() const {
				return _depth;
			}
			std::vector<colorAttachment> getColors() const {
				return _colors;
			}

			static void unbind() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			frameBuffer(std::vector<colorAttachment> colors, depthTexture* depth, unsigned int width, unsigned int height)
				: _depth(depth), _colors(colors), _width(width), _height(height) {
				glGenFramebuffers(1, &_ptr);
				glBindFramebuffer(GL_FRAMEBUFFER, _ptr);

				std::vector<GLenum> attachments;
				for (colorAttachment color : colors) {
					GLenum attachment = GL_COLOR_ATTACHMENT0 + color.attachment;
					glFramebufferTexture(GL_FRAMEBUFFER, attachment, color.tex->ptr(), 0);
					attachments.push_back(attachment);
				}

				if (depth) {
					glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->ptr(), 0);
				}

				glDrawBuffers(GLsizei(attachments.size()), attachments.data());
				setViewport(0, 0, _width, _height);
			}
			frameBuffer(window* win) : _width(win->framebufferSize().x), _height(win->framebufferSize().y) {
				_ptr = 0;
				_depth = nullptr;
				setViewport(0, 0, _width, _height);
			}
			~frameBuffer() {
				if(_ptr != 0) glDeleteFramebuffers(1, &_ptr);
			}
		private:
			frameBuffer(const frameBuffer& other) = delete;
			frameBuffer& operator=(const frameBuffer& other) = delete;
		};
	}
}

#endif