#include <graphics/window/window.h>
#include <util/graphics_h.h>

#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#include <map>


namespace citrus {
	namespace graphics {
		//hardcoded icon
		int const iconData[64 * 64 * 4 / sizeof(int)] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,1567651071,1332046335,1332046335,1332046335,1399352575,1920695551,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,1955171839,476805887,25863423,9810431,9810431,9810431,9810431,10073599,9480959,4674815,85137407,623520767,1768516351,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,1233819391,176857855,10468607,9217535,239621119,1197368063,1803452927,1803452927,1803452927,1685617919,1014136063,379237375,883803391,43298815,9612799,10073599,1519887359,0,0,0,0,0,0,
			0,0,0,0,0,0,0,696488959,10073599,9612799,38295295,608126463,0,0,0,0,0,0,0,0,0,0,-656350721,-1649232129,612471551,8032767,943801343,0,0,0,0,0,
			0,0,0,0,0,979989247,10534399,8954367,222580479,1381653759,0,0,0,228706047,564710143,531155711,1169675263,833539839,0,0,0,0,0,0,-774383361,479768575,8691199,910510335,0,0,0,0,
			0,0,0,0,745174527,11126783,139748351,1516002815,0,0,0,0,0,833539839,933741823,1620748799,766365183,1623316991,0,0,0,0,0,0,0,-808003585,176462847,8888575,1937802239,0,0,0,
			0,0,0,1652326911,11192831,240476671,0,0,0,0,0,0,0,1052037631,1722136063,1684301055,884002815,0,0,0,867225599,1102435071,564710143,984797439,0,0,-1565741313,24743935,76194815,0,0,0,
			0,0,0,25336319,7176959,0,0,0,1942543871,1052037631,430295295,0,0,833539839,2025375743,1499488767,682413311,0,0,1236849919,228706047,430295295,379898111,884002815,1841749247,0,0,1600810495,9810431,695435775,0,0,
			0,0,1079467007,10271231,1163879679,0,0,1220072447,1102435071,1035194879,833539839,1388107519,0,682413311,2042087167,1482909439,564710143,0,1774509055,531155711,648727807,884002815,1959057663,312723455,1035194879,0,0,-387389185,189684735,26192383,0,0,
			0,0,173170687,8888575,0,0,1724177407,867225599,833539839,833539839,1236849919,1959321087,0,1320867071,1974781183,1600810495,1707334399,0,766365183,480758271,1219941119,2073402367,1768516351,564710143,228706047,1724177407,0,0,1837336575,10271231,1130127871,0,
			0,0,9810431,593719295,0,0,1623316991,833539839,1218689535,1349219071,2042679807,867225599,1102435071,0,1958530559,1617653759,1959321087,1102435071,94291199,1337512703,1836875775,1246448895,1537455615,480758271,228706047,766365183,0,0,-1178878977,25336319,358246143,0,
			0,828731647,10073599,1870166527,0,2110578943,682413311,564710143,-1394874369,2071690239,1903260159,0,564710143,0,-1630544897,1365206015,1472059391,682413311,1051642623,1752463359,1431919615,1891356415,648727807,312723455,984797439,984797439,0,0,-387389185,123695359,91523583,0,
			0,76589567,56191487,0,0,1808129279,379898111,984797439,1539299583,-1058804737,1819045119,1987475199,0,1892146687,0,1515870975,0,1404225791,1499488767,1650944255,1404225791,379898111,94291199,984797439,0,0,0,0,0,373838591,8691199,0,
			2140379135,10073599,759515647,0,0,0,0,1808129279,430295295,984797439,-873860865,-1920102913,1617258239,1286654463,-1209930497,0,0,1280398079,1551400959,1186123519,1623316991,0,0,0,0,0,0,0,0,845047551,8954367,0,
			1635154687,9810431,1399352575,0,0,0,0,0,0,0,1942543871,-1798052865,-1566136321,1129535487,0,0,-522133249,-1532713729,0,0,0,0,0,0,0,0,0,0,0,1148156159,8954367,2088533247,
			-1246053377,580168191,1432840703,0,0,0,0,0,0,0,0,0,-1899176705,0,0,0,0,2090705919,1788980223,1788980223,1889182463,1922341631,1922341631,1938658047,1938658047,1955171839,1955040255,1955040255,1955171839,694974975,8888575,1414812927,
			0,2071887871,892745471,1414812927,1414812927,1414812927,1414812927,1414812927,1179010815,1381126911,1414812927,1414812927,1414812927,1414812927,1028016127,9612799,10073599,749519871,967557375,1472321023,1942739967,1186121471,9810431,10073599,262257919,951176447,749519871,749519871,749519871,481348607,5464831,1633772031,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1986355713,10534399,507858687,1485741055,1937802239,1633772031,1246448895,1246448895,1280398079,1213618687,1415866367,2089389055,-2020436737,-2020436737,-1936287489,-1936287489,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1565478145,9810431,2089389055,1707334399,951176447,2110578943,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1514883329,9480959,-2037872129,-991432705,713531135,697280767,682347775,312723455,1102435071,2110578943,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1547910913,25336319,2088533247,-319292673,1202702847,995647487,-1699102465,833539839,531155711,379898111,766365183,1724177407,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1665021953,7440127,2071690239,0,-2133925633,715111679,1011831807,-1615545857,1220072447,228706047,312723455,682413311,867225599,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1698510337,7176959,2071690239,0,-1176244481,430295295,933412351,1196183039,-1598636801,766365183,94291199,178308607,984797439,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1731932929,6715903,2071690239,0,-470550529,648727807,312723455,0,1280398079,-1850426113,766365183,178308607,1623316991,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1731932929,6584063,2071690239,0,-84017665,1320867071,430295295,833539839,1370474495,611485439,564380671,766365183,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1765355521,6584063,2088533247,0,0,-1461917185,228706047,884002815,648727807,564710143,564710143,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1866084353,6584063,0,0,0,-268895233,984797439,379898111,312723455,1169675263,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,-2084319489,6584063,0,0,0,-84017665,-1176244481,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,1824180735,6584063,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,1572588799,22308095,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,1522850047,87112447,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,1203227903,136521983,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};


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
			GLFWimage img;
			img.height = 32;
			img.width = 32;
			img.pixels = (unsigned char*)iconData;
			glfwSetWindowIcon(_win, 1, &img);
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
