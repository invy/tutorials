#include "Texture.h"

#include "WaylandCompositor.h"

void Texture::texture_create (int width, int height, void *data)
{
    m_width = width;
    m_height = height;
    glGenTextures (1, &m_identifier);
    glBindTexture (GL_TEXTURE_2D, m_identifier);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture (GL_TEXTURE_2D, 0);
}

void Texture::texture_create_from_egl_image (int width, int height, EGLImage image)
{
    m_width = width;
    m_height = height;
    glGenTextures (1, &m_identifier);
    glBindTexture (GL_TEXTURE_2D, m_identifier);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    getSetup()->glEGLImageTargetTexture2DOES (GL_TEXTURE_2D, image);
    glBindTexture (GL_TEXTURE_2D, 0);
}

void Texture::texture_draw (int x, int y)
{
    if (!m_identifier)
        return;

    GLint vertices[] = {
        x, y,
        x+m_width, y,
        x+m_width, y+m_height,
        x, y+m_height
    };
    GLint tex_coords[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };

    glEnable (GL_TEXTURE_2D);
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glBindTexture (GL_TEXTURE_2D, m_identifier);
    glVertexPointer (2, GL_INT, 0, vertices);
    glTexCoordPointer (2, GL_INT, 0, tex_coords);

    glDrawArrays (GL_QUADS, 0, 4);

    glBindTexture (GL_TEXTURE_2D, 0);
    glDisable (GL_TEXTURE_RECTANGLE);
    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

void Texture::texture_delete ()
{
    if (m_identifier)
    {
        glDeleteTextures (1, &m_identifier);
    }
}
