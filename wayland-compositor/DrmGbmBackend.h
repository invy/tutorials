#pragma once

#include "IBackend.h"

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GL/gl.h>

class DrmGbmBackend : public IBackend
{
public:
    DrmGbmBackend();
    virtual ~DrmGbmBackend();
    // IBackend interface
public:
    void backend_init(Callbacks *callbacks);
    EGLDisplay backend_get_egl_display();
    void backend_swap_buffers();
    void backend_dispatch_nonblocking();
    void backend_wait_for_events(int wayland_fd);
    void backend_get_keymap(int *fd, int *size);
    long backend_get_timestamp();

private:
    void setup_opengl();
    void clean_up ();
    drmModeEncoder* find_encoder(drmModeRes* resources, drmModeConnector* connector);
    drmModeConnector* find_connector(drmModeRes* resources);
    void find_display_configuration();

private:
    EGLDisplay m_eglDisplay;
    Callbacks m_callbacks;
    int m_device;

    uint32_t connector_id;
    drmModeModeInfo mode_info;
    drmModeCrtc *crtc;

    struct gbm_device* gbm_device;
    EGLContext context;
    struct gbm_surface* gbm_surface;
    EGLSurface egl_surface;

    struct gbm_bo *previous_bo { nullptr };
    uint32_t previous_fb;
};

