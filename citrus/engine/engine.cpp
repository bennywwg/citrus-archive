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
		//if (ed) ed->eng = this;

		Log("Render Thread Begin");
		_renderState.store(render_halted);

		//try {
			_renderState.store(render_initializing);
			_win = nullptr;
			_win = new graphics::window(1280, 720, "Citrus Engine", "C:\\Users\\benny\\Desktop\\citrus\\res");

			try {
				sys = new graphics::system(*_win->inst(), resDir / "textures", resDir / "meshes", resDir / "animations");
				fpath shaderPath = resDir / "shaders" / "build";
				graphics::frameStore* fs = new graphics::frameStore(*_win->inst());
				graphics::meshPass* mp = new graphics::meshPass(*sys, fs, true, true, false, shaderPath / "standard.vert.spv", shaderPath / "standard.frag.spv");
				//graphics::meshPass* bp = new graphics::meshPass(*sys, fs, true, true, true,  shaderPath / "bones.vert.spv", shaderPath / "bones.frag.spv");
				graphics::finalPass* fp = new graphics::finalPass(*sys, *_win, *mp, shaderPath / "finalPass.vert.spv", shaderPath / "finalPass.frag.spv");
				fp->addDependency(mp);
				//fp->addDependency(bp);
				
				sys->setFinalPass(fp);
			} catch (std::runtime_error const& re) {
				util::sout(string(re.what()) + "\n");
				throw re;
			}
			//graphics::image4b img(string("C:\\Users\\benny\\Desktop\\citrus\\res") + "/textures/grid.png");

			//graphics::ctTexture tx = _win->inst()->createTexture4b(img.width(), img.height(), false, img.data());

			//_win->inst()->_finalPass->setTexture0(tx);
			//_win->inst()->_finalPass->buildAllCommandBuffers();

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
				/*if (!ed || ed->playing || ed->doFrame)*/ man->preRender();
				man->render();

				sys->render();

				fpsSample++;

				long long nextLastFrame = (clock::now() - fbegin).count();
				while(((clock::now() - fbegin).count() + lastFrameNanos) <= (long long)(_timeStep * 1000000000)) { }
				lastFrameNanos = nextLastFrame;
				
				/*if (!ed || ed->playing || ed->doFrame)*/ frame++;
					
				//_win->inst()->destroySemaphore(imageReadySem);
				//_win->inst()->destroySemaphore(frameDoneSem);

				//if (ed) ed->doFrame = false;
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

		//_win->inst()->destroyTexture(tx);

		delete sys;

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
		ed = nullptr;
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
