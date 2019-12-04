#pragma once

#include <string>
#include <map>
#include <functional>

#include <glm/ext.hpp>
#include <iostream>
#include <vector>

#include "instance.h"

struct GLFWwindow;

namespace citrus {
		class window;

        class instance;
        
		using std::string;


		namespace windowInput {
			enum button {
				none = -1,
				leftMouse = 0,
				rightMouse = 1,
				middleMouse = 2,
				a = 3,
				b = 4,
				c = 5,
				d = 6,
				e = 7,
				f = 8,
				g = 9,
				h = 10,
				i = 11,
				j = 12,
				k = 13,
				l = 14,
				m = 15,
				n = 16,
				o = 17,
				p = 18,
				q = 19,
				r = 20,
				s = 21,
				t = 22,
				u = 23,
				v = 24,
				w = 25,
				x = 26,
				y = 27,
				z = 28,
				lshift = 29,
				rshift = 30,
				tab = 31,
				enter = 32,
				arrowRight = 33,
				arrowUp = 34,
				arrowLeft = 35,
				arrowDown = 36,
				space = 37,
				key0 = 38,
				key1 = 39,
				key2 = 40,
				key3 = 41,
				key4 = 42,
				key5 = 43,
				key6 = 44,
				key7 = 45,
				key8 = 46,
				key9 = 47,
				minus = 48,
				equals = 49,
				openBracket = 50,
				closeBracket = 51,
				backslash = 52,
				comma = 53,
				period = 54,
				slash = 55,
				tilde = 56,
				back = 57,
				semicolon = 58,
				apostrophe = 59,
				escape = 60,
				home = 61,
				end = 62,
				pgup = 63,
				pgdn = 64,
				del = 65,

				ctr_lbump = 102,
				ctr_rbump = 103,
				ctr_ltrigger = 104,
				ctr_rtrigger = 105,
				ctr_east = 106,
				ctr_north = 107,
				ctr_west = 108,
				ctr_south = 109,
				ctr_select = 110,
				ctr_start = 111,
				ctr_dpad_east = 112,
				ctr_dpad_north = 113,
				ctr_dpad_west = 114,
				ctr_dpad_south = 115,
			};
			enum analog {
				ctr_invalid = 0,
				ctr_l = 1,
				ctr_r = 2,
				ctr_lstick_x = 3,
				ctr_lstick_y = 4,
				ctr_rstick_x = 5,
				ctr_rstick_y = 6
			};

			char toChar(button bu, bool shifted);
		}

		extern std::map<GLFWwindow*, std::function<void(windowInput::button, int, int)>> _buttonCallbackTable;
		//extern std::map<GLFWwindow*, std::function<void(int, int, int, int)>> _mouseButtonCallbackTable;
		extern std::map<GLFWwindow*, std::function<void(double, double)>> _cursorCallbackTable;
		extern std::map<GLFWwindow*, window*> _windowTable;

		class window {
			const static unsigned int _numButtonEnums = 300; // could be smaller but eh
			public:


			static void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
			static void buttonCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
			static void cursorCallback(GLFWwindow* win, double x, double y);
			
			private:
			GLFWwindow* _win;
			string _adapter;

			glm::dvec2 _cursorPos;
			int _buttonStates[_numButtonEnums];
            
            instance *_inst;

			public:
			bool shouldClose();

			bool getKey(windowInput::button but);
			glm::dvec2 getCursorPos();

			float controlleValue(windowInput::analog a);
			bool  controllerButton(windowInput::button b);
			std::vector<std::string> controllers();

			glm::ivec2 framebufferSize();

            uint32_t getNextFrameIndex(VkSemaphore imageReadySignal);
			void present(uint32_t index, VkSemaphore wait);
			void present(uint32_t index, std::vector<VkSemaphore> waits);

			void poll();
			instance* inst();

			void setButtonCallback(std::function<void(windowInput::button, int, int)> func);
			void setCursorCallback(std::function<void(double, double)> func);
			void removeCallbacks();

			string getAdapter();

			window(unsigned int width, unsigned int height, std::string title, std::string resFolder = "");
			~window();

			private:
			window(const window&) = delete;
			window& operator=(const window&) = delete;
		};
}