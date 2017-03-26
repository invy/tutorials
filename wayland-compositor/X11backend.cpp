
#include "X11backend.h"

#include <wayland-server.h>
#include <X11/Xlib.h>
#include <linux/input.h>
#include <EGL/egl.h>
#include <X11/Xlib-xcb.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <poll.h>

void X11Backend::create_window (void) {
	// setup EGL
	EGLint attribs[] = {
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	EGL_NONE};
	EGLConfig config;
	EGLint num_configs_returned;
    eglChooseConfig (m_eglDisplay, attribs, &config, 1, &num_configs_returned);
	
	// get the visual from the EGL config
	EGLint visual_id;
    eglGetConfigAttrib (m_eglDisplay, config, EGL_NATIVE_VISUAL_ID, &visual_id);
	XVisualInfo visual_template;
	visual_template.visualid = visual_id;
	int num_visuals_returned;
	XVisualInfo *visual = XGetVisualInfo (x_display, VisualIDMask, &visual_template, &num_visuals_returned);
	
	// create a window
    XSetWindowAttributes window_attributes;
	window_attributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask;
    window_attributes.colormap = XCreateColormap (x_display, RootWindow(x_display,DefaultScreen(x_display)), visual->visual, AllocNone);
	window.window = XCreateWindow (
		x_display,
        RootWindow(x_display, DefaultScreen(x_display)),
		0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		0, // border width
		visual->depth, // depth
		InputOutput, // class
		visual->visual, // visual
		CWEventMask|CWColormap, // attribute mask
		&window_attributes // attributes
	);
	
	// EGL context and surface
	eglBindAPI (EGL_OPENGL_API);
    window.context = eglCreateContext (m_eglDisplay, config, EGL_NO_CONTEXT, NULL);
    window.surface = eglCreateWindowSurface (m_eglDisplay, config, window.window, NULL);
    eglMakeCurrent (m_eglDisplay, window.surface, window.surface, window.context);
	
	XFree (visual);
	
	XMapWindow (x_display, window.window);
}

X11Backend::X11Backend()
{
    printf("creating X11Backend\n");
}

void X11Backend::backend_init (Callbacks *_callbacks)
{
    printf("initializing backend\n");

    m_callbacks = *_callbacks;
	x_display = XOpenDisplay (NULL);
	
	xcb_connection = XGetXCBConnection (x_display);
	struct xkb_context *context = xkb_context_new (XKB_CONTEXT_NO_FLAGS);
    xkb_x11_setup_xkb_extension (xcb_connection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
                                 xkb_x11_setup_xkb_extension_flags::XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
                                 NULL, NULL, NULL, NULL);
	keyboard_device_id = xkb_x11_get_core_keyboard_device_id (xcb_connection);
	keymap = xkb_x11_keymap_new_from_device (context, xcb_connection, keyboard_device_id, XKB_KEYMAP_COMPILE_NO_FLAGS);
	state = xkb_x11_state_new_from_device (keymap, xcb_connection, keyboard_device_id);
	
    m_eglDisplay = eglGetDisplay (x_display);
    eglInitialize (m_eglDisplay, NULL, NULL);
	create_window ();
}

EGLDisplay X11Backend::backend_get_egl_display (void) {
    return m_eglDisplay;
}

void X11Backend::backend_swap_buffers (void) {
    eglSwapBuffers (m_eglDisplay, window.surface);
}

void X11Backend::update_modifiers (void) {
	struct modifier_state modifier_state;
	modifier_state.depressed = xkb_state_serialize_mods (state, XKB_STATE_MODS_DEPRESSED);
	modifier_state.latched = xkb_state_serialize_mods (state, XKB_STATE_MODS_LATCHED);
	modifier_state.locked = xkb_state_serialize_mods (state, XKB_STATE_MODS_LOCKED);
	modifier_state.group = xkb_state_serialize_layout (state, XKB_STATE_LAYOUT_EFFECTIVE);
    m_callbacks.modifiers (modifier_state);
}

void X11Backend::backend_dispatch_nonblocking (void) {
	XEvent event;
	while (XPending(x_display)) {
		XNextEvent (x_display, &event);
		if (event.type == ConfigureNotify) {
            m_callbacks.resize (event.xconfigure.width, event.xconfigure.height);
		}
		else if (event.type == Expose) {
            m_callbacks.draw ();
		}
		else if (event.type == MotionNotify) {
            m_callbacks.mouse_motion (event.xbutton.x, event.xbutton.y);
		}
		else if (event.type == ButtonPress) {
			if (event.xbutton.button == Button1)
                m_callbacks.mouse_button (BTN_LEFT, WL_POINTER_BUTTON_STATE_PRESSED);
			else if (event.xbutton.button == Button2)
                m_callbacks.mouse_button (BTN_MIDDLE, WL_POINTER_BUTTON_STATE_PRESSED);
			else if (event.xbutton.button == Button3)
                m_callbacks.mouse_button (BTN_RIGHT, WL_POINTER_BUTTON_STATE_PRESSED);
		}
		else if (event.type == ButtonRelease) {
			if (event.xbutton.button == Button1)
                m_callbacks.mouse_button (BTN_LEFT, WL_POINTER_BUTTON_STATE_RELEASED);
			else if (event.xbutton.button == Button2)
                m_callbacks.mouse_button (BTN_MIDDLE, WL_POINTER_BUTTON_STATE_RELEASED);
			else if (event.xbutton.button == Button3)
                m_callbacks.mouse_button (BTN_RIGHT, WL_POINTER_BUTTON_STATE_RELEASED);
		}
		else if (event.type == KeyPress) {
            m_callbacks.key (event.xkey.keycode - 8, WL_KEYBOARD_KEY_STATE_PRESSED);
			xkb_state_update_key (state, event.xkey.keycode, XKB_KEY_DOWN);
			update_modifiers ();
		}
		else if (event.type == KeyRelease) {
            m_callbacks.key (event.xkey.keycode - 8, WL_KEYBOARD_KEY_STATE_RELEASED);
			xkb_state_update_key (state, event.xkey.keycode, XKB_KEY_UP);
			update_modifiers ();
		}
		else if (event.type == FocusIn) {
			xkb_state_unref (state);
			state = xkb_x11_state_new_from_device (keymap, xcb_connection, keyboard_device_id);
			update_modifiers ();
		}
	}
}

void X11Backend::backend_wait_for_events (int wayland_fd) {
	struct pollfd fds[2] = {{ConnectionNumber(x_display), POLLIN}, {wayland_fd, POLLIN}};
	poll (fds, 2, -1);
}

void X11Backend::backend_get_keymap (int *fd, int *size) {
	char *string = xkb_keymap_get_as_string (keymap, XKB_KEYMAP_FORMAT_TEXT_V1);
	*size = strlen (string) + 1;
	char *xdg_runtime_dir = getenv ("XDG_RUNTIME_DIR");
	*fd = open (xdg_runtime_dir, O_TMPFILE|O_RDWR|O_EXCL, 0600);
	ftruncate (*fd, *size);
    char *map = static_cast<char*>(mmap (NULL, *size, PROT_READ|PROT_WRITE, MAP_SHARED, *fd, 0));
	strcpy (map, string);
	munmap (map, *size);
	free (string);
}

long X11Backend::backend_get_timestamp (void) {
	struct timespec t;
	clock_gettime (CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
