#include <iostream>

#include "../mankern/manager.inl"
#include "../mankern/elementRef.inl"

#include "../graphkern/window.h"
#include "../graphkern/clearFrame.h"
#include "../graphkern/meshPass.h"
#include "../graphkern/immediatePass.h"
#include "../graphkern/finalPass.h"

#include "../builtin/common_ele.h"

#include "playerController.h"

#define CT_USE_EDITOR

#ifdef CT_USE_EDITOR
#include "../editor/editor.h"
#endif

#include <irrKlang.h>

using namespace citrus;

class engineProper {
public:
	fpath resDir;

	bool edEnabled = true;

	manager man;
	window win;

	bool closed;

	renderSystem* sys;
	frameStore* fs = nullptr;
	meshPass* mp = nullptr, * bp = nullptr;
	finalPass* fp = nullptr;
	clearFrame* cf = nullptr;
	immediatePass* ip = nullptr;
	immediatePass* wip = nullptr;

	world* wd = nullptr;

	double t;

#ifdef CT_USE_EDITOR
	uint16_t lastSelectedIndex;
	ctEditor* ed;
#endif

	inline void buttonCB(windowInput::button b, int mode, int mods) {
		if (mode == 1 || mode == 2) ed->keyDown(b);
		if (mode == 1 && b == windowInput::escape) edEnabled = !edEnabled;
		else ed->keyUp(b);
	}

	inline void run() {
		win.setButtonCallback([this](windowInput::button b, int v2, int v3) {buttonCB(b, v2, v3); });

		man.registerType<shapeEle>("shapeEle", true, wd);

		man.registerType<collisionEle>("collisionEle", true, wd);

		man.registerType<rigidEle>("rigidEle", true, wd);

		man.registerType<sensorEle>("sensorEle", true, wd);

		man.registerType<freeCam>("freeCam", true, sys);

		man.registerType<playerController>("playerController", true, &win);

		modelEleStruct st;
		st.sys = sys;
		st.time = &t;
		man.registerType<modelEle>("modelEle", true, &st);

		man.addElement<freeCam>(man.create("cam"));

		while (!closed && !man.stopped()) {
			bool stepFrame = true;
#ifdef CT_USE_EDITOR
			stepFrame = ed->playing;
#endif // CT_USE_EDITOR

			if (stepFrame) {
				man.flushToDestroy();
				man.flushToCreate();
				man.action();
			}

			sys->frameCam.aspectRatio = 1.6f / 0.9f;

			wd->step();
			render();
			
			if (win.shouldClose()) {
				closed = true;
			}

			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	inline void render() {
		win.poll();

#ifdef CT_USE_EDITOR
		if (edEnabled) {
			cf->cursorX = (int)win.getCursorPos().x;
			if (cf->cursorX >= win.framebufferSize().x) cf->cursorX = 0;
			cf->cursorY = win.framebufferSize().y - (int)win.getCursorPos().y - 1;
			if (cf->cursorY >= win.framebufferSize().y) cf->cursorY = 0;

			ed->cursorPx = win.getCursorPos();
			ed->cursor = vec2(win.getCursorPos()) / vec2(win.framebufferSize()) * 2.0f - 1.0f;
			ed->cursor.y = -ed->cursor.y;

			ed->render(*ip);
			man.renderGUI(ip->groupings);
			if (win.getKey(windowInput::leftMouse)) {
				if (!ed->dragged) {
					ed->startDragPx = ed->cursorPx;
					ed->startDrag = ed->cursor;
					ed->mouseDown(lastSelectedIndex);
				}
				ed->dragged = true;
			} else {
				if (ed->dragged) {
					ed->mouseUp(lastSelectedIndex);
				}
				ed->dragged = false;
			}

			wd->debugDraw();
		}
#endif // CT_USE_EDITOR

		sys->render();

#ifdef CT_USE_EDITOR
		if (edEnabled) {
			lastSelectedIndex = cf->selectedIndex;
			ed->update(*ip, lastSelectedIndex);
		} else {
			ip->groupings.clear();
			wip->groupings.clear();
		}
#endif
	}

	inline engineProper() : resDir(R"(C:\Users\benny\Build\citrus\res)"), win(1280, 720, "Citrus 2", resDir.u8string()) {

		irrklang::ISoundEngine* e = irrklang::createIrrKlangDevice();
		char * audioPath = (char *)malloc(500);
		strcpy(audioPath, (resDir / "audio" / "bells-tibetan-daniel_simon.mp3").string().c_str());
		auto src = e->addSoundSourceFromFile(audioPath, irrklang::ESM_AUTO_DETECT, true);
		e->play2D(src);



		try {
			fpath shaderPath = resDir / "shaders" / "build";
			sys = new renderSystem(*win.inst(), resDir / "textures", resDir / "meshes", resDir / "animations");
			fs = new frameStore(*win.inst());
			mp = new meshPass(*sys, fs, true, true, false, shaderPath / "standard.vert.spv", shaderPath / "standard.frag.spv", false);
			bp = new meshPass(*sys, fs, true, true, true, shaderPath / "bones.vert.spv", shaderPath / "bones.frag.spv", false);
			ip = new immediatePass(*sys, fs, shaderPath / "immediate.vert.spv", shaderPath / "immediate.frag.spv", false, false);
			wip = new immediatePass(*sys, fs, shaderPath / "immediateWire.vert.spv", shaderPath / "immediateWire.frag.spv", true, true);
			fp = new finalPass(*sys, win, *fs, shaderPath / "finalPass.vert.spv", shaderPath / "finalPass.frag.spv");
			cf = new clearFrame(*sys, fs);

			mp->addDependency(cf); mp->initialIndex = (1 << 14) + 1;
			bp->addDependency(cf); bp->initialIndex = (1 << 14) + (1 << 13) + 1;
			ip->addDependency(cf); ip->indexBits = (1 << 15);
			wip->addDependency(cf); wip->addDependency(ip); wip->indexBits = (1 << 13);

			fp->addDependency(mp);
			fp->addDependency(bp);
			fp->addDependency(ip);
			fp->addDependency(wip);

			sys->passes = { cf, mp, bp, ip, wip, fp };

			wd = new world(*wip);
		} catch (std::runtime_error const& re) {
			std::cout << string(re.what()) << "\n";
			throw re;
		}
#ifdef CT_USE_EDITOR
		lastSelectedIndex = 0;
		ed = new ctEditor(&man, sys, &win);
		_globalEd = ed;
#endif
	}
	~engineProper() {
#ifdef CT_USE_EDITOR
		delete ed;
#endif
		delete cf;
		delete fp;
		delete ip;
		delete wip;
		delete bp;
		delete mp;
		delete fs;
		delete sys;
	}
};

int main(int main, char** argv) {
	srand(65836441);

	glfwInit();

	engineProper p;

	p.run();

	return 0;
}