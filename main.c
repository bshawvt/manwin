#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "X11/Xlib.h"
#include "X11/Xlibint.h"
//#include "X11/Xresource.h"
#include "X11/Xutil.h"

//#include "parse.h"
//#include "helpers.h"
#include "wm.h"
int main(int argc, char *argv[]) {
	Log("hello wm world: %i %i", sizeof(int), sizeof(short int));
	WMState *state = initWM(false);
	Log("wm state ready");
	Log("screens: %i", XScreenCount(state->display));
	//int cycle = 0;
	//XWarpPointer(state->display, state->rootWindow, state->rootWindow, 1, 1, 10, 10, 10, 10);
	while(state->shutdown == 0) {
		XNextEvent(state->display, &state->event);
		WMEventState inputState = getEventState(state);
		switch (inputState.type) {
			case KeyPress: {
				// todo
				break;
			}
			case KeyRelease: {
				if (inputState.inputCode == state->fnActivateKeycode) {
					XRaiseWindow(state->display, inputState.eventWindow);
					XSetInputFocus(state->display, inputState.eventWindow, RevertToParent, CurrentTime);
				}
				else if (inputState.inputCode == state->fnCloseKeycode) {
					Log("todo: handle close window hotkey better");
					closeWindow(state, inputState.eventWindow);
					
				}
				else if (inputState.inputCode == state->fnActivateNextKeycode) {
					Log("todo: the alt(super)+tab hotkey is not implemented");
					/*
						what i know so far:
							all windows are children of root
							all child windows of children of root are also children of root
							XQueryTree root and parent will always be same
							argh!
					*/
					/*WMWindows subs = getRootSubwindows(state);
					Log("length: %i", subs.length);
					if (subs.length == 0)
						break;
					if ((inputState.modifiers & ShiftMask) == ShiftMask) { // reverse order
						state->cycle = (state->cycle + subs.length - 1) % subs.length;
					}
					else {
						state->cycle = (state->cycle + 1) % subs.length;
					}*/
					//Log("activated window id: %i %i", subs.windows[state->cycle - 1], state->cycle);
					//getWindows(state, subs.windows[state->cycle]);
					//activateWindow(state, subs.windows[state->cycle]);
				};
				break;
			}
			case ButtonPress: {
				state->lastEventState = inputState;
				state->lastEventState.active = true;
				break;
			}
			case ButtonRelease: {
				state->lastEventState = (WMEventState) {0};
				state->lastEventState.active = false; // todo
				break;
			}
			case MotionNotify: {
				if (state->lastEventState.active) {
					int x = state->lastEventState.eventRect.x;
					int y = state->lastEventState.eventRect.y;
					int w = state->lastEventState.eventRect.width;
					int h = state->lastEventState.eventRect.height;
					if (state->lastEventState.inputCode == state->fnMoveButton) {
						x += inputState.x - state->lastEventState.x;
						y += inputState.y - state->lastEventState.y;
					}
					else if (state->lastEventState.inputCode == state->fnResizeButton) {
						w += inputState.x - state->lastEventState.x;
						h += inputState.y - state->lastEventState.y;
					}
					// todo: these clamps are probably very wrong
					XMoveResizeWindow(state->display,
									  state->lastEventState.eventWindow,
									  x,//ClampI(x, 1, 10000),
									  y,//ClampI(y, 1, 10000),
									  ClampI(w, 1, 10000),
									  ClampI(h, 1, 10000));
				}
				break;
			}
			default: {
				Log("unhandled x event type: %i", inputState.type);
				break;
			}
		};
	};
	cleanWM(state); // send save your self / shutdown messages or whatever maybe?
	return 1;
};