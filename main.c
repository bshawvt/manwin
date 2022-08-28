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
	Log("hello wm world");
	WMState *state = initWM();
	Log("wm state ready");
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
					Log("todo: handle close window hotkey");
				}
				break;
			}
			case ButtonPress: {
				state->lastEventState = inputState;
				state->lastEventState.active = true;
				break;
			}
			case ButtonRelease: {
				state->lastEventState = (WMEventState) {0};
				state->lastEventState.active = false;
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
}