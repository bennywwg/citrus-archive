#include "window.h"

#include <util/graphics_h.h>
#include <GLFW/glfw3.h>
#include <util/stdUtil.h>

namespace citrus {
	namespace graphics {
		std::map<GLFWwindow*, std::function<void(int, int, int, int)>> _buttonCallbackTable;
		std::map<GLFWwindow*, std::function<void(double, double)>> _cursorCallbackTable;
		std::map<GLFWwindow*, window*> _windowTable;

		void window::buttonCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
			using namespace windowInput;
			button but = none;
			if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
				but = button(key - GLFW_KEY_A + a);
			} else if(key == GLFW_KEY_LEFT_SHIFT) {
				but = lshift;
			} else if(key == GLFW_KEY_RIGHT_SHIFT) {
				but = rshift;
			} else if(key == GLFW_KEY_TAB) {
				but = tab;
			} else if(key == GLFW_KEY_ENTER) {
				but = enter;
			} else if(key == GLFW_KEY_RIGHT) {
				but = arrowRight;
			} else if(key == GLFW_KEY_UP) {
				but = arrowUp;
			} else if(key == GLFW_KEY_LEFT) {
				but = arrowLeft;
			} else if(key == GLFW_KEY_DOWN) {
				but = arrowDown;
			} else if(key == GLFW_KEY_SPACE) {
				but = space;
			} else if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
				but = button(key - GLFW_KEY_0 + key0);
			} else if(key == GLFW_KEY_MINUS) {
				but = minus;
			} else if(key == GLFW_KEY_EQUAL) {
				but = equals;
			} else if(key == GLFW_KEY_LEFT_BRACKET) {
				but = openBracket;
			} else if(key == GLFW_KEY_RIGHT_BRACKET) {
				but = closeBracket;
			} else if(key == GLFW_KEY_BACKSLASH) {
				but = backslash;
			} else if(key == GLFW_KEY_COMMA) {
				but = comma;
			} else if(key == GLFW_KEY_PERIOD) {
				but = period;
			} else if(key == GLFW_KEY_SLASH) {
				but = slash;
			} else if(key == GLFW_KEY_GRAVE_ACCENT) {
				but = tilde;
			} else if(key == GLFW_KEY_BACKSPACE) {
				but = back;
			} else if(key == GLFW_KEY_SEMICOLON) {
				but = semicolon;
			} else if(key == GLFW_KEY_APOSTROPHE) {
				but = apostrophe;
			} else if(key == GLFW_KEY_ESCAPE) {
				but = escape;
			}
			if(key != none) {
				_windowTable[win]->_buttonStates[but] = action != GLFW_RELEASE;
			}
			
			_buttonCallbackTable[win](key, scancode, action, mods);
		}
		void window::cursorCallback(GLFWwindow* win, double x, double y) {
			_windowTable[win]->_cursorPos = glm::dvec2(x, y);
			_cursorCallbackTable[win](x, y);
		}

		bool window::shouldClose() {
			return glfwWindowShouldClose(_win);
		}
		bool window::getKey(windowInput::button but) {
			return _buttonStates[but];
		}
		glm::dvec2 window::getCursorPos() {
			return _cursorPos;
		}
		glm::ivec2 window::framebufferSize() {
			int width, height;
			glfwGetFramebufferSize(_win, &width, &height);
			return glm::ivec2(width, height);
		}
		void window::swapBuffers() {
			glfwSwapBuffers(_win);
		}
		void window::poll() {
			glfwPollEvents();
		}
		void window::setButtonCallback(std::function<void(int, int, int, int)> func) {
			_buttonCallbackTable[_win] = func;
		}
		void window::setCursorCallback(std::function<void(double, double)> func) {
			_cursorCallbackTable[_win] = func;
		}
		void window::removeCallbacks() {
			_buttonCallbackTable.erase(_win);
			_cursorCallbackTable.erase(_win);
		}
		window::window(unsigned int width, unsigned int height, std::string title) {
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			_win = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
			_windowTable[_win] = this;
			glfwSetKeyCallback(_win, buttonCallback);
			setButtonCallback([](int, int, int, int){ });
			glfwSetCursorPosCallback(_win, cursorCallback);
			setCursorCallback([](double, double) { });

			glfwMakeContextCurrent(_win);

			GLenum error = glGetError();

			glewExperimental = true;
			GLenum code = glewInit();

			glfwSwapInterval(0);

			util::sout("Using GPU: " + std::string((char*)glGetString(GL_RENDERER)));
		}
		window::window() {
			glfwDestroyWindow(_win);
		}
	}
}