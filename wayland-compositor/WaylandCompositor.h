#pragma once

#include "Texture.h"
#include "Surface.h"
#include "Client.h"

#include <wayland-server.h>
#include <GL/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) (GLenum target, EGLImage image);

class EGLSetup
{
public:
    EGLSetup(PFNEGLBINDWAYLANDDISPLAYWL _eglBindWaylandDisplayWL,
             PFNEGLQUERYWAYLANDBUFFERWL _eglQueryWaylandBufferWL,
             PFNGLEGLIMAGETARGETTEXTURE2DOESPROC _glEGLImageTargetTexture2DOES)
        : eglBindWaylandDisplayWL(_eglBindWaylandDisplayWL),
          eglQueryWaylandBufferWL(_eglQueryWaylandBufferWL),
          glEGLImageTargetTexture2DOES(_glEGLImageTargetTexture2DOES)
    {
    }

public:
    PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL;
    PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
};

extern EGLSetup* getSetup();

static Client *get_client (struct wl_client *_client);
static void deactivate_surface (Surface *surface);
static void activate_surface (Surface *surface);
static void delete_surface (struct wl_resource *resource);

// surface
static void surface_destroy (struct wl_client *client, struct wl_resource *resource);
static void surface_attach (struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer, int32_t x, int32_t y);
static void surface_damage (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
static void surface_frame (struct wl_client *client, struct wl_resource *resource, uint32_t callback);
static void surface_set_opaque_region (struct wl_client *client, struct wl_resource *resource, struct wl_resource *region);
static void surface_set_input_region (struct wl_client *client, struct wl_resource *resource, struct wl_resource *region);
static void surface_commit (struct wl_client *client, struct wl_resource *resource);
static void surface_set_buffer_transform (struct wl_client *client, struct wl_resource *resource, int32_t transform);
static void surface_set_buffer_scale (struct wl_client *client, struct wl_resource *resource, int32_t scale);

// region
static void region_destroy (struct wl_client *client, struct wl_resource *resource);
static void region_add (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
static void region_subtract (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);

// compositor
static void compositor_create_surface (struct wl_client *client, struct wl_resource *resource, uint32_t id);
static void compositor_create_region (struct wl_client *client, struct wl_resource *resource, uint32_t id);

static void compositor_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id);

// shell surface
static void shell_surface_pong (struct wl_client *client, struct wl_resource *resource, uint32_t serial);
static void shell_surface_move (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial);
static void shell_surface_resize (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, uint32_t edges);
static void shell_surface_set_toplevel (struct wl_client *client, struct wl_resource *resource);
static void shell_surface_set_transient (struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags);
static void shell_surface_set_fullscreen (struct wl_client *client, struct wl_resource *resource, uint32_t method, uint32_t framerate, struct wl_resource *output);
static void shell_surface_set_popup (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags);
static void shell_surface_set_maximized (struct wl_client *client, struct wl_resource *resource, struct wl_resource *output);
static void shell_surface_set_title (struct wl_client *client, struct wl_resource *resource, const char *title);
static void shell_surface_set_class (struct wl_client *client, struct wl_resource *resource, const char *class_);

// wl shell
static void shell_get_shell_surface (struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface);

static void shell_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id);

// xdg surface
static void xdg_surface_destroy (struct wl_client *client, struct wl_resource *resource);
static void xdg_surface_set_parent (struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent);
static void xdg_surface_set_title (struct wl_client *client, struct wl_resource *resource, const char *title);
static void xdg_surface_set_app_id (struct wl_client *client, struct wl_resource *resource, const char *app_id);
static void xdg_surface_show_window_menu (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y);
static void xdg_surface_move (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial);
static void xdg_surface_resize (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, uint32_t edges);
static void xdg_surface_ack_configure (struct wl_client *client, struct wl_resource *resource, uint32_t serial);
static void xdg_surface_set_window_geometry (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
static void xdg_surface_set_maximized (struct wl_client *client, struct wl_resource *resource);
static void xdg_surface_unset_maximized (struct wl_client *client, struct wl_resource *resource);

static void xdg_surface_set_fullscreen (struct wl_client *client, struct wl_resource *resource, struct wl_resource *output);
static void xdg_surface_unset_fullscreen (struct wl_client *client, struct wl_resource *resource);
static void xdg_surface_set_minimized (struct wl_client *client, struct wl_resource *resource);

// xdg shell
static void xdg_shell_destroy (struct wl_client *client, struct wl_resource *resource);
static void xdg_shell_use_unstable_version (struct wl_client *client, struct wl_resource *resource, int32_t version);

static void xdg_shell_get_xdg_surface (struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *_surface);
static void xdg_shell_get_xdg_popup (struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface, struct wl_resource *parent, struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y);
static void xdg_shell_pong (struct wl_client *client, struct wl_resource *resource, uint32_t serial);

static void xdg_shell_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id);

// pointer
static void pointer_set_cursor (struct wl_client *client, struct wl_resource *resource, uint32_t serial, struct wl_resource *_surface, int32_t hotspot_x,
int32_t hotspot_y);
static void pointer_release (struct wl_client *client, struct wl_resource *resource);


// keyboard
static void keyboard_release (struct wl_client *client, struct wl_resource *resource);

// seat
static void seat_get_pointer (struct wl_client *client, struct wl_resource *resource, uint32_t id);
static void seat_get_keyboard (struct wl_client *client, struct wl_resource *resource, uint32_t id);
static void seat_get_touch (struct wl_client *client, struct wl_resource *resource, uint32_t id);

static void seat_bind (struct wl_client *client, void *data, uint32_t version, uint32_t id);
// backend callbacks
static void handle_resize_event (int width, int height);
static void handle_draw_event (void);
static void handle_mouse_motion_event (int x, int y);
static void handle_mouse_button_event (int button, int state);
static void handle_key_event (int key, int state);
static void handle_modifiers_changed (struct modifier_state new_state);
static void draw (void);
