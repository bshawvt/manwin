#ifndef MY_WM_H
#define MY_WM_H

#include "helpers.h"
#define MAX_CHILD_WINDOWS 2048

typedef int (*MyErrorHandler) (Display*, XErrorEvent*); // stinky callback for stinky x error handling
typedef struct WMWindows {
	Window root;
	Window parent;
	Window windows[MAX_CHILD_WINDOWS];
	unsigned int length;
} WMWindows;
typedef struct WMRect {
	int x;
	int y;
	int width;
	int height;
} WMRect;
typedef struct WMEventState {
	bool root; // did event happen on root window?
	bool active; // used as way to save hotkey state
	bool keyboard; // XEvent.type == KeyPress, ..., KeyRelease
	bool mouse; // XEvent.type == Button1, Button2, ..., Button5, ..., MotionNotify
	int type; // XEvent.type
	unsigned int modifiers;
	unsigned int inputCode; //xbutton.button or xkey.keycode that generated event
	unsigned int x; // x_root
	unsigned int y; // y_root
	Window eventWindow; // window id that generated this event?
	WMRect eventRect; // basic geometry of eventWindow
} WMEventState;
typedef struct WMState {
	Window rootWindow; // parent window
	Window wmWindow; // this window
	Window children[MAX_CHILD_WINDOWS]; // children windows created by window manager
	WMWindows allWindows;
	int cycle; // iterate through all windows
	int nextIndex; // child element index
	int childrenLength; // current number of subwindows
	bool shutdown; // exit main loop
	WMEventState lastEventState;
	// super + keycode combination to trigger X events
	unsigned int fnCloseKeycode;
	unsigned int fnActivateKeycode;
	unsigned int fnActivateNextKeycode;
	unsigned int fnResizeButton;
	unsigned int fnMoveButton;
	// x stuff
	Display *display;
	XEvent event;
} WMState;

/* initWM 
	sets the programs initial state
	*/
WMState* initWM(bool noHotkeys);
/* createWindow 
	will create and add a reference of child window(including the window manager window) to state
	*/
Window createWindow(WMState *state, char* title, int x, int y, int width, int height, Window parent);
/* activateWindow
	basically a wrapper for XRaiseWindow
	*/
void activateWindow(WMState *state, Window window);
/* showWindow 
	will attempt to map and then raise agwindow to the front 
	*/
void showWindow(WMState *state, Window window);
/* killWindow 
	kills any x client and then removes the child reference from state if applicable 
	*/
void killWindow(WMState *state, Window window);
/* getWindowRect
	returns a WMRect with the windows basic geometry
*/
WMRect getWindowRect(WMState *state, Window window);
/* getEventState
	normalizes data from X events
	*/
WMEventState getEventState(WMState *state);
/* setHotkeyTrigger
	generate x events when a hotkey is activated
	*/
void setHotkeyTrigger(WMState *state, unsigned int activateKeycode, unsigned int additionalMod, bool overrideSuper);
/* unsetHotkeyTrigger
	stop generating x events when a hotkey is activated
	*/
void unsetHotkeyTrigger(WMState *state, unsigned int activateKeycode, unsigned int additionalMod, bool overrideSuper);

/* getRootSubwindows
	a very slow procedure that returns a list of all window ids
	*/
WMWindows getRootSubwindows(WMState *state);

/* getWindows
	another very slow procedure that returns another list of window ids
	*/
WMWindows getWindows(WMState *state, Window window);
	
WMWindows getWindows(WMState *state, Window window) {
	//Window root;
	//Window parent;
	//Window *windows;
	//unsigned int size;
	WMWindows w = {0};
	
	//Log("w.root: %i, w.parent: %i, w.length: %i", w.root, w.parent, size);
	//Log("I belong to: %i %i %i %i", window, w.root,  w.parent, size);*/
	Log("looking at %i", window);
	if (window != None && window != state->wmWindow && window != state->rootWindow) {
		XWMHints *hints = XGetWMHints(state->display, window);
		
		//Display *display;
		//Window w;
		
		//long long_offset, long_length;
		//Bool delete;
		//Atom req_type; 
		/*Atom *actual_type_return;
		int *actual_format_return;
		unsigned long *nitems_return;
		unsigned long *bytes_after_return;
		unsigned char **prop_return;*/
		
		Atom type;
		int format;
		
		unsigned long items;
		unsigned long bytes;
		unsigned char *prop;
		
		int r = XGetWindowProperty(state->display, 
						   window, 
						   XInternAtom(state->display, "_NET_NM_NAME", False),
						   0,
						   65536,
						   false,
						   XInternAtom(state->display, "UTF8_STRING", False),
						   &type,
						   &format,
						   &items,
						   &bytes, 
						   &prop);
		Log("test %i %i %i %i %i %s", r, type, format, items, bytes, prop);
		XFree(prop);
		//Log("has hints?: %i", hints);
		//XWindowAttributes attributes;
		//XGetWindowAttributes(state->display, window, &attributes);
		//Log("%i %i %i %i", attributes.x, attributes.y, attributes.width, attributes.height);
		//XMapWindow(state->display, window);
		if (hints) {
			/*Window *windows;
			unsigned int size;
			XQueryTree(state->display,
			   window,
			   &w.root,
			   &w.parent,
			   &windows,
			   &size);*/
			//Log("window_group: %i", hints->window_group);
			/*XWindowAttributes attributes;
			XGetWindowAttributes(state->display, window, &attributes);
			Log(" %i %i", attributes.width, attributes.x);*/
			//Log("root: %i parent: %i id: %i", w.root, w.parent, window);
			//XFree(windows);
		}
		XFree(hints);
	}
	//XFree(windows);
	return w;
};
WMWindows getRootSubwindows(WMState *state) {
	Window root;
	Window parent;
	Window *windows;
	unsigned int size;
	XQueryTree(state->display,
			   state->rootWindow,
			   &root,
			   &parent,
			   &windows,
			   &size);
	Log("chillens? %i", size);
	if (size > MAX_CHILD_WINDOWS) {
		Log("assert: todo: a call to XQueryTree has exceeded MAX_CHILD_WINDOWS!!!");
	};
	WMWindows w = (WMWindows) {0};
	w.root = root;
	w.parent = parent;
	w.length = 0;
	Log("size: %i", size);
	for(int x = 0; x < size - 1; x++) {
		//WMWindows sub = getWindows(state, windows[x]);
		/*if (sub.parent != state->rootWindow) {
			Log("this isn't a child of root?!");
		};*/
		getWindows(state, windows[x]);
	};
	//XRestackWindows(state->display, windows, size);
	//Log("w.root: %i, w.parent: %i, w.length: %i", w.root, w.parent, w.length);
	/*for(int x = 0; x < size; x++) { // add only the good ones
		w.windows[x] = windows[x];
	};*/
	XFree(windows);
	return w;
};

WMEventState getEventState(WMState *state) {
	WMEventState event = (WMEventState) {0};
	event.keyboard = (state->event.type == KeyPress || state->event.type == KeyRelease);
	event.mouse = (state->event.type == ButtonPress || state->event.type == ButtonRelease || state->event.type == MotionNotify);
	if (event.keyboard) {
		event.eventWindow = state->event.xkey.subwindow;
		event.inputCode = state->event.xkey.keycode;
		event.modifiers = state->event.xkey.state;
	}
	else if (event.mouse) {
		event.eventWindow = state->event.xbutton.subwindow;
		event.inputCode = state->event.xbutton.button;
		event.x = state->event.xbutton.x_root;
		event.y = state->event.xbutton.y_root;
		event.modifiers = state->event.xbutton.state;
	};
	if (event.eventWindow == None)
		event.eventWindow = state->rootWindow;
	event.type = state->event.type;
	event.active = false;
	event.root = (state->rootWindow == event.eventWindow);
	event.eventRect = (WMRect) {0};
	event.eventRect = getWindowRect(state, event.eventWindow);
	return event;
};
int someHandler(Display *d, XErrorEvent *err) {
	char buffer[1028];
	XGetErrorText(d, err->error_code, buffer, 1028);
	Log("error: %i %i\n%s", err->error_code, err->request_code, buffer);
	return 0;
};
WMState* initWM(bool noHotkeys) {
	WMState *state = malloc(sizeof(WMState));
	*state = (WMState) {0};
	state->cycle = 0;
	state->display = XOpenDisplay(0x0);
	XSetErrorHandler(someHandler);
	// set hotkey keycode definitions
	state->fnActivateNextKeycode = XKeysymToKeycode(state->display, XK_Tab);
	state->fnActivateKeycode = XKeysymToKeycode(state->display, XK_F1);
	state->fnCloseKeycode = XKeysymToKeycode(state->display, XK_F4);
	state->fnMoveButton = Button1;
	state->fnResizeButton = Button3;
	state->rootWindow = XDefaultRootWindow(state->display);
	Log("root window id: %i", state->rootWindow);
	//setHotkeyTrigger(state, state->fnSuperLKeycode, AnyModifier, true);
	if (!noHotkeys) {
		setHotkeyTrigger(state, state->fnActivateNextKeycode, 0, false);
		setHotkeyTrigger(state, state->fnActivateNextKeycode, ShiftMask, false);
		setHotkeyTrigger(state, state->fnActivateKeycode, 0, false);
		setHotkeyTrigger(state, state->fnCloseKeycode, 0, false);
		setHotkeyTrigger(state, state->fnMoveButton, 0, false);
		setHotkeyTrigger(state, state->fnResizeButton, 0, false);
	}

	// if this was wayland it should be named "yutani window manager" because alien 1 & 2 were the best movies
	state->wmWindow = createWindow(state, "glue and spaghetti window manager\0", 10, 10, 100, 100, state->rootWindow);
	XChangeProperty(state->display,
					state->wmWindow,
					XInternAtom(state->display, "_NET_SUPPORTING_WM_CHECK", False),
					XInternAtom(state->display, "WINDOW", False),
					32,
					PropModeReplace, 
					(unsigned char*) &state->wmWindow,
					1);
	XChangeProperty(state->display,
					state->rootWindow,
					XInternAtom(state->display, "_NET_SUPPORTING_WM_CHECK", False),
					XInternAtom(state->display, "WINDOW", False),
					32,
					PropModeReplace, 
					(unsigned char*) &state->wmWindow,
					1);
	//showWindow(state, state->wmWindow);
	return state;
};
void cleanWM(WMState *state) {
	// unset hotkeys in case initWM is called again?
	unsetHotkeyTrigger(state, state->fnCloseKeycode, 0, false);
	unsetHotkeyTrigger(state, state->fnActivateKeycode, 0, false);
	unsetHotkeyTrigger(state, state->fnActivateNextKeycode, 0, false);
	unsetHotkeyTrigger(state, state->fnActivateNextKeycode, ShiftMask, false);
	unsetHotkeyTrigger(state, state->fnMoveButton, 0, false);
	unsetHotkeyTrigger(state, state->fnResizeButton, 0, false);
	// cleanup children windows created by the wm?
	for(int i = 0; i < MAX_CHILD_WINDOWS; i++) {
		if (state->children[i] != 0)
			killWindow(state, state->children[i]);
	};
	XCloseDisplay(state->display);
	free(state);
	state = NULL;
};
Window createWindow(WMState *state, char *title, int x, int y, int width, int height, Window parent) {
	if (parent == None)
		parent = state->rootWindow;
	int borderWidth = 1;
	int depth = CopyFromParent;
	int wClass = InputOutput;
	int valueMask = 0;

	Visual visual = {0};
	XSetWindowAttributes attributes = {0};		// = malloc(sizeof(XSetWindowAttributes));
	attributes.background_pixmap = None;		/* background, None, or ParentRelative */
	attributes.background_pixel = 0x00FF00;		/* background pixel */
	attributes.border_pixmap = CopyFromParent;	/* border of the window or CopyFromParent */
	attributes.border_pixel = 0xFF0000;			/* border pixel value */
	attributes.bit_gravity = ForgetGravity;		/* one of bit gravity values */
	attributes.win_gravity = NorthWestGravity;	/* one of the window gravity values */
	attributes.backing_store = NotUseful;		/* NotUseful, WhenMapped, Always */
	attributes.backing_planes = 1;				/* planes to be preserved if possible */
	attributes.backing_pixel = 0;				/* value to use in restoring planes */
	attributes.save_under = False;				/* should bits under be saved? (popups) */
	attributes.event_mask = 0;					/* set of events that should be saved */
	attributes.do_not_propagate_mask = 0;		/* set of events that should not propagate */
	attributes.override_redirect = False;		/* boolean value for override_redirect */
	attributes.colormap = CopyFromParent;		/* color map to be associated with window */
	attributes.cursor = None;

	Window win = XCreateWindow(state->display, 
						 parent, 
						 x, 
						 y, 
						 width, 
						 height, 
						 borderWidth, 
						 depth, 
						 wClass, 
						 &visual, 
						 valueMask, 
						 &attributes);

	XChangeProperty(state->display,
					win,
					XInternAtom(state->display, "_NET_WM_NAME", False),
					XInternAtom(state->display, "UTF8_STRING", False),
					8,
					PropModeAppend, 
					(unsigned char*) title,
					strlen(title));
	state->children[state->nextIndex++] = win;
	state->childrenLength++;
	return win;
};
void showWindow(WMState *state, Window window) {
	XMapWindow(state->display, window);
	//XMapRaised(state->display, window);
	XRaiseWindow(state->display, window);
	//XClearWindow(state->display, window);
	//XFlush(state->display);
};
void activateWindow(WMState *state, Window window) {
	XRaiseWindow(state->display, window);
};
// kill wm children windows;
void killWindow(WMState *state, Window window) {
	for(int i = 0; i < MAX_CHILD_WINDOWS; i++) { // do some child window record keeping
		if (state->children[i] == window) {
			state->children[i] = None;
			state->childrenLength--;
			break;
		};
	};
	XKillClient(state->display, window);
	XFlush(state->display);
};
//XSetErrorHandler(errorHandler);
WMRect getWindowRect(WMState *state, Window window) {
	XWindowAttributes attr;
	XGetWindowAttributes(state->display, window, &attr);
	WMRect rect;
	rect.x = attr.x;
	rect.y = attr.y;
	rect.width = attr.width;
	rect.height = attr.height;
	return rect;
};
void setHotkeyTrigger(WMState *state, unsigned int activateKeycode, unsigned int additionalMod, bool overrideSuper) {
	// hack for capslock/numlock causing hotkeys to not work and giving me anxiety
	int maskNum = 4;
	unsigned int superMask = Mod4Mask;
	if (overrideSuper) {
		superMask = 0;
		maskNum = 1;
	};
	unsigned int masks[] = {
		(superMask|additionalMod),
		(superMask|additionalMod)|Mod2Mask,
		(superMask|additionalMod)|LockMask,
		(superMask|additionalMod)|LockMask|Mod2Mask
	};
	for(int i = 0; i < maskNum; i++) {
		if (activateKeycode < 6) // xlib defines mouse buttons as 1-5 so keyboard keycodes will always be greater
			XGrabButton(state->display,
				activateKeycode,//Button3,
				masks[i],
				state->rootWindow,
				True,
				ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
				GrabModeAsync,
				GrabModeAsync,
				None,
				None);
		else
			XGrabKey(state->display,
				activateKeycode,//state->fnActivateKeycode,
				masks[i],
				state->rootWindow, 
				True, 
				GrabModeAsync, 
				GrabModeAsync);
	};
};
void unsetHotkeyTrigger(WMState *state, unsigned int activateKeycode, unsigned int additionalMod, bool overrideSuper) {
	int maskNum = 4;
	unsigned int superMask = Mod4Mask;
	if (overrideSuper) {
		superMask = overrideSuper;
		maskNum = 1;
	};
	unsigned int masks[] = {
		(superMask|additionalMod),
		(superMask|additionalMod)|Mod2Mask,
		(superMask|additionalMod)|LockMask,
		(superMask|additionalMod)|LockMask|Mod2Mask
	};
	for(int i = 0; i < maskNum; i++) {
		if (activateKeycode < 6)
			XUngrabButton(state->display,
			   activateKeycode,
			   masks[i],
			   state->rootWindow);
		else
			XUngrabKey(state->display, 
			  activateKeycode,
			  masks[i],
			  state->rootWindow);
	};
};
#endif