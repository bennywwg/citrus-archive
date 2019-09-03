#include <typeindex>
#include "citrus/util.h"

#include "citrus/engine/engine.h"
#include "citrus/graphics/window.h"
#include "citrus/editor/gui.h"
#include <vector>
#include "citrus/graphics/system/finalPass.h"
#include "citrus/graphics/image.h"
#include "citrus/graphics/system/meshPass.h"
#include "citrus/graphics/system/finalPass.h"
#include "citrus/graphics/system/clearFrame.h"
#include "citrus/graphics/system/immediatePass.h"
#include "citrus/editor/editor.h"

namespace citrus::engine {
	void engine::Log(string str) {
		std::lock_guard<std::mutex> lock(_logMut);
		_log.emplace_back(this->time(), str);
	}
	void engine::Log(std::vector<string> strs) {
		std::lock_guard<std::mutex> lock(_logMut);
		for(auto str : strs) {
			_log.emplace_back(this->time(), str);
		}
	}

	vector<pair<double, string>> engine::flushLog() {
		std::lock_guard<std::mutex> lock(_logMut);
		auto res = _log;
		_log.clear();
		return res;
	}

	void engine::_runRender() {
		if (ed) ed->eng = this;

		Log("Render Thread Begin");
		_renderState.store(render_halted);

		//try {
			_renderState.store(render_initializing);
			_win = nullptr;
			_win = new graphics::window(1728, 972, "Citrus Engine", "C:\\Users\\benny\\Desktop\\citrus\\res");

			graphics::frameStore* fs = nullptr;
			graphics::meshPass* mp = nullptr, *bp = nullptr;
			graphics::finalPass* fp = nullptr;
			graphics::clearFrame* cf = nullptr;
			graphics::immediatePass* ip = nullptr;

			try {
				fpath shaderPath = resDir / "shaders" / "build";
				sys = new graphics::system(*_win->inst(), resDir / "textures", resDir / "meshes", resDir / "animations");
				fs = new graphics::frameStore(*_win->inst());
				mp = new graphics::meshPass(*sys, fs, true, true, false, shaderPath / "standard.vert.spv", shaderPath / "standard.frag.spv", false);
				bp = new graphics::meshPass(*sys, fs, true, true, true,  shaderPath / "bones.vert.spv", shaderPath / "bones.frag.spv", false);
				ip = new graphics::immediatePass(*sys, fs, shaderPath / "immediate.vert.spv", shaderPath / "immediate.frag.spv", true);
				fp = new graphics::finalPass(*sys, *_win, *fs, shaderPath / "finalPass.vert.spv", shaderPath / "finalPass.frag.spv");
				cf = new graphics::clearFrame(*sys, fs);

				mp->addDependency(cf); mp->initialIndex = (1 << 14)             + 1;
				bp->addDependency(cf); bp->initialIndex = (1 << 14) + (1 << 13) + 1;
				ip->addDependency(cf);

				fp->addDependency(mp);
				fp->addDependency(bp);
				fp->addDependency(ip);
				
				sys->passes = { cf, mp, bp, ip, fp };
			} catch (std::runtime_error const& re) {
				util::sout(string(re.what()) + "\n");
				throw re;
			}

			this->Log(_win->getAdapter());

			Log(_win->controllers());
			
			_renderState.store(render_doingRender);
			clock::time_point fpsSampleStart = clock::now();
			int fpsSample = 0;

			long long lastFrameNanos = 0;

			while(!stopped()) {
				if (_win->shouldClose()) {
					stop();
					continue;
				}

				if((clock::now() - fpsSampleStart) >= std::chrono::seconds(1)) {
					fpsSampleStart = fpsSampleStart + std::chrono::seconds(1);
					_framesPerSecond = fpsSample;
					fpsSample = 0;
				}

				clock::time_point fbegin = clock::now();

				_win->poll();

				man->flush();
				if (!ed || ed->playing || ed->doFrame) man->preRender();
				man->render();

				cf->cursorX = (int)_win->getCursorPos().x;
				if (cf->cursorX >= _win->framebufferSize().x) cf->cursorX = 0;
				cf->cursorY = _win->framebufferSize().y - (int)_win->getCursorPos().y - 1;
				if (cf->cursorY >= _win->framebufferSize().y) cf->cursorY = 0;

				ed->render(*ip);
				if (_win->getKey(graphics::windowInput::leftMouse)) {
					if (!ed->dragged) {
						ed->startDragPx = _win->getCursorPos();
						ed->click(_win->getCursorPos());
					}
					ed->dragged = true;
				} else {
					ed->dragged = false;
				}

				sys->render();

				uint16_t selectedIndex = cf->selectedIndex;

				for (int i = 0; i < sys->meshPasses.size(); i++) {
					sys->meshPasses[i]->items - sys->meshPasses[i]->initialIndex;
				}

				fpsSample++;

				long long nextLastFrame = (clock::now() - fbegin).count();
				//while(((clock::now() - fbegin).count() + lastFrameNanos) <= (long long)(_timeStep * 1000000000)) { }
				lastFrameNanos = nextLastFrame;
				
				if (!ed || ed->playing || ed->doFrame) frame++;

				if (ed) ed->doFrame = false;
			}
		/*} catch(std::runtime_error ex) {
			Log("Unrecoverable Error in Render Thread: " + std::string(ex.what()));
		} catch(eleDereferenceException ex) {
			Log("Unrecoverable Error in Render Thread: " + std::string(ex.er));
		} catch(entDereferenceException ex) {
			Log("Unrecoverable Error in Render Thread: " + std::string(ex.er));
		} catch(...) {
			Log("Unrecoverable Error in Render Thread (Whack error.)");
		}*/

		if (mp) delete mp;
		if (cf) delete cf;
		if (ip) delete ip;
		if (bp) delete bp;
		if (fp) delete fp;
		if (fs) delete fs;
		if(sys) delete sys;

		if(_win) delete _win;

		_renderState.store(render_finished);
		Log("Render Thread End");
	}

	graphics::window* engine::getWindow() const {
		return _win;
	}
	
	int engine::fps() {
		return _framesPerSecond;
	}

	engine::renderState engine::getRenderState() {
		return _renderState.load();
	}

	double engine::time() {
		return frame * _timeStep;
	}

	double engine::dt() {
		return _timeStep;
	}

	float engine::controllerValue(graphics::windowInput::analog a) {
		return _win->controlleValue(a);
	}
	bool engine::controllerButton(graphics::windowInput::button b) {
		return _win->controllerButton(b);
	}
	bool engine::getKey(graphics::windowInput::button but) {
		return _win->getKey(but);
	}

	void engine::loadLevel(fpath levelPath) {
		/*for(auto& it : std::experimental::filesystem::directory_iterator(levelPath)) {
			if(!std::experimental::filesystem::is_directory(it.path())) {
				std::string path = it.path().string();
				try {
					std::string read = util::loadEntireFile(path);
					json parsed = json::parse(read);
					std::string type = parsed["type"].get<std::string>();

				} catch(std::exception ex) {
					Log("Couldn't load resource " + path + ", " + ex.what() + "\n");
				}
			}
		}*/
	}

	void engine::setOrder(vector<std::type_index> order) {
		man->setOrder(order);
	}

	
	void engine::start() {
		_engineStart = clock::now();
		_renderThread = std::thread(&engine::_runRender, this);
	}
	void engine::stop() {
		_closed = true;
	}
	bool engine::stopped() {
		return _closed;
	}

	engine::engine(double timeStep) : _timeStep(timeStep) {
		ed = new editor::ctEditor();
		_closed = false;
		_renderState = render_halted;
		_engineStart = clock::now();
		man = new manager(this);
		Log("Engine Start");
	}
	engine::~engine() {
		stop();
		_renderThread.join();
		Log("Engine Stop");
	}
}
