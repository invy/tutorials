#pragma once

#include <EGL/egl.h>

struct modifier_state
{
	uint32_t depressed, latched, locked, group;
};

struct Callbacks
{
	void (*resize) (int width, int height);
	void (*draw) (void);
	void (*mouse_motion) (int x, int y);
	void (*mouse_button) (int button, int state);
	void (*key) (int key, int state);
	void (*modifiers) (struct modifier_state modifier_state);
};

class IBackend {
public:
    virtual ~IBackend() { }
public:
    virtual void backend_init (Callbacks *callbacks) = 0;
    virtual EGLDisplay backend_get_egl_display (void) = 0;
    virtual void backend_swap_buffers (void) = 0;
    virtual void backend_dispatch_nonblocking (void) = 0;
    virtual void backend_wait_for_events (int wayland_fd) = 0;
    virtual void backend_get_keymap (int *fd, int *size) = 0;
    virtual long backend_get_timestamp (void) = 0;
};

