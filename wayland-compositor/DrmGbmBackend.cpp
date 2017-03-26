#include "DrmGbmBackend.h"


#include <gbm.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

DrmGbmBackend::DrmGbmBackend()
{

}

DrmGbmBackend::~DrmGbmBackend()
{
    clean_up ();
    close (m_device);
}

drmModeConnector* DrmGbmBackend::find_connector (drmModeRes *resources)
{
        // iterate the connectors
        int i;
        for (i=0; i<resources->count_connectors; i++) {
                drmModeConnector *connector = drmModeGetConnector (m_device, resources->connectors[i]);
                // pick the first connected connector
                if (connector->connection == DRM_MODE_CONNECTED) {
                        return connector;
                }
                drmModeFreeConnector (connector);
        }
        // no connector found
        return nullptr;
}

drmModeEncoder* DrmGbmBackend::find_encoder (drmModeRes *resources, drmModeConnector *connector)
{
        if (connector->encoder_id) {
                return drmModeGetEncoder (m_device, connector->encoder_id);
        }
        // no encoder found
        return nullptr;
}

void DrmGbmBackend::find_display_configuration ()
{
        drmModeRes *resources = drmModeGetResources (m_device);
        // find a connector
        drmModeConnector *connector = find_connector (resources);
        if (!connector)
        {
            fputs("no connector found\n", stderr);
            exit(1);
        }
        // save the connector_id
        connector_id = connector->connector_id;
        // save the first mode
        mode_info = connector->modes[0];
        printf ("resolution: %ix%i\n", mode_info.hdisplay, mode_info.vdisplay);
        // find an encoder
        drmModeEncoder *encoder = find_encoder (resources, connector);
        if (!encoder)
        {
            fputs("no encoder found\n", stderr);
            exit(1);
        }
        // find a CRTC
        if (encoder->crtc_id)
        {
                crtc = drmModeGetCrtc (m_device, encoder->crtc_id);
        }
        drmModeFreeEncoder (encoder);
        drmModeFreeConnector (connector);
        drmModeFreeResources (resources);
}

void DrmGbmBackend::clean_up ()
{
        // set the previous crtc
        drmModeSetCrtc (m_device, crtc->crtc_id, crtc->buffer_id, crtc->x, crtc->y, &connector_id, 1, &crtc->mode);
        drmModeFreeCrtc (crtc);

        if (previous_bo) {
                drmModeRmFB (m_device, previous_fb);
                gbm_surface_release_buffer (gbm_surface, previous_bo);
        }

        eglDestroySurface (m_eglDisplay, egl_surface);
        gbm_surface_destroy (gbm_surface);
        eglDestroyContext (m_eglDisplay, context);
        eglTerminate (m_eglDisplay);
        gbm_device_destroy (gbm_device);
}

void DrmGbmBackend::setup_opengl()
{
        gbm_device = gbm_create_device (m_device);
        m_eglDisplay = eglGetDisplay (reinterpret_cast<EGLNativeDisplayType>(gbm_device));
        eglInitialize (m_eglDisplay, NULL, NULL);

        // create an OpenGL context
        eglBindAPI (EGL_OPENGL_API);
        EGLint attributes[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
        EGL_NONE};
        EGLConfig config;
        EGLint num_config;
        eglChooseConfig (m_eglDisplay, attributes, &config, 1, &num_config);
        context = eglCreateContext (m_eglDisplay, config, EGL_NO_CONTEXT, NULL);

        // create the GBM and EGL surface
        gbm_surface = gbm_surface_create (gbm_device, mode_info.hdisplay, mode_info.vdisplay, GBM_BO_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);

        egl_surface = eglCreateWindowSurface(m_eglDisplay,
                                             config,
                                             reinterpret_cast<EGLNativeWindowType>(gbm_surface),
                                             nullptr);
        eglMakeCurrent (m_eglDisplay, egl_surface, egl_surface, context);
}

void DrmGbmBackend::backend_init(Callbacks *callbacks)
{
    m_callbacks = *callbacks;

    m_device = open ("/dev/dri/card0", O_RDWR|O_CLOEXEC);
    find_display_configuration ();
    setup_opengl ();
}

EGLDisplay DrmGbmBackend::backend_get_egl_display()
{
    return m_eglDisplay;
}

void DrmGbmBackend::backend_swap_buffers()
{
    eglSwapBuffers (m_eglDisplay, egl_surface);
    struct gbm_bo *bo = gbm_surface_lock_front_buffer (gbm_surface);
    uint32_t handle = gbm_bo_get_handle (bo).u32;
    uint32_t pitch = gbm_bo_get_stride (bo);
    uint32_t fb;
    drmModeAddFB (m_device, mode_info.hdisplay, mode_info.vdisplay, 24, 32, pitch, handle, &fb);
    drmModeSetCrtc (m_device, crtc->crtc_id, fb, 0, 0, &connector_id, 1, &mode_info);

    if (previous_bo)
    {
            drmModeRmFB (m_device, previous_fb);
            gbm_surface_release_buffer (gbm_surface, previous_bo);
    }
    previous_bo = bo;
    previous_fb = fb;
}

void DrmGbmBackend::backend_dispatch_nonblocking()
{

}

void DrmGbmBackend::backend_wait_for_events(int wayland_fd)
{

}

void DrmGbmBackend::backend_get_keymap(int *fd, int *size)
{

}

long DrmGbmBackend::backend_get_timestamp()
{
    struct timespec t;
    clock_gettime (CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
