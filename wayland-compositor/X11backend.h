#pragma once
#include "IBackend.h"

#include <X11/Xlib-xcb.h>

class X11Backend : public IBackend {
public:
    X11Backend();
public:
    void backend_init (Callbacks *callbacks) override;
    EGLDisplay backend_get_egl_display (void) override;
    void backend_swap_buffers (void) override;
    void backend_dispatch_nonblocking (void) override;
    void backend_wait_for_events (int wayland_fd) override;
    void backend_get_keymap (int *fd, int *size) override;
    long backend_get_timestamp (void) override;

private:
    void create_window (void);
    void update_modifiers (void);

private:
    struct {
            Window window;
            EGLContext context;
            EGLSurface surface;
    } window;

    Display *x_display;
    EGLDisplay m_eglDisplay;
    Callbacks m_callbacks;
    xcb_connection_t *xcb_connection;
    int32_t keyboard_device_id;
    struct xkb_keymap *keymap;
    struct xkb_state *state;

    uint32_t WINDOW_WIDTH {800};
    uint32_t WINDOW_HEIGHT {600};

};

