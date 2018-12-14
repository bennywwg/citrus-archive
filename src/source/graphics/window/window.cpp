#include <graphics/window/window.h>
#include <util/graphics_h.h>

#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#include <map>


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
			if(but != none) {
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
		bool window::controllerButton(windowInput::button b) {
			if(!glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				return false;
			}

			int count = 0;
			const unsigned char* states = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

			switch(b) {
				case windowInput::ctr_invalid:
					return false;
				case windowInput::button::ctr_east:
					return count >= 1 && states[1];
				case windowInput::button::ctr_north:
					return count >= 3 && states[3];
				case windowInput::button::ctr_west:
					return count >= 2 && states[2];
				case windowInput::button::ctr_south:
					return count >= 0 && states[0];
				case windowInput::button::ctr_ltrigger:
					return count >= 4 && states[4];
				case windowInput::button::ctr_rtrigger:
					return count >= 5 && states[5];
				case windowInput::button::ctr_select:
					return count >= 6 && states[6];
				case windowInput::button::ctr_start:
					return count >= 7 && states[7];
				case windowInput::button::ctr_lbump:
					return count >= 8 && states[8];
				case windowInput::button::ctr_rbump:
					return count >= 9 && states[9];
				case windowInput::button::ctr_dpad_east:
					return count >= 12 && states[12];
				case windowInput::button::ctr_dpad_north:
					return count >= 13 && states[13];
				case windowInput::button::ctr_dpad_west:
					return count >= 11 && states[11];
				case windowInput::button::ctr_dpad_south:
					return count >= 10 && states[10];
				default:
					return false;
			}
		}
		std::vector<std::string> window::controllers() {
			if(glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				return {std::string(glfwGetJoystickName(GLFW_JOYSTICK_1))};
			} else {
				return {};
			}
		}
		float window::controlleValue(windowInput::analog a) {
			if(!glfwJoystickPresent(GLFW_JOYSTICK_1)) {
				return 0.0f;
			}

			int count = 0;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

			switch(a) {
				case citrus::graphics::windowInput::ctr_invalid:
					return 0.0f;
				case citrus::graphics::windowInput::ctr_l:
					return count >= 4 ? axes[4] : 0.0f;
				case citrus::graphics::windowInput::ctr_r:
					return count >= 5 ? axes[4] : 0.0f;
				case citrus::graphics::windowInput::ctr_lstick_x:
					return count >= 0 ? axes[0] : 0.0f;
				case citrus::graphics::windowInput::ctr_lstick_y:
					return count >= 1 ? axes[1] : 0.0f;
				case citrus::graphics::windowInput::ctr_rstick_x:
					return count >= 2 ? axes[2] : 0.0f;
				case citrus::graphics::windowInput::ctr_rstick_y:
					return count >= 3 ? axes[3] : 0.0f;
				default:
					return 0.0f;
			}
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
		string window::getAdapter() {
			return _adapter;
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

			memset(_buttonStates, 0, sizeof(_buttonStates));

			_adapter = std::string((char*)glGetString(GL_RENDERER));
		}
		window::~window() {
			glfwDestroyWindow(_win);
		}
	}
}
