#pragma once

#include <GL/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

class Texture {
public:
    GLuint m_identifier;
    int m_width;
    int m_height;

    void texture_create (int width, int height, void *data);
    void texture_create_from_egl_image (int width, int height, EGLImage image);
    void texture_draw (int x, int y);
    void texture_delete ();

};

