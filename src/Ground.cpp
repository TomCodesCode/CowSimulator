#include "Ground.h"
#include "stb_image.h"
#include <cstdio>

Ground::Ground(const char* texPath) {
    texId = loadTexture(texPath);
    // print error if loading failed
    if (!texId) {
        std::fprintf(stderr, "[Ground] Failed to load texture: %s\n", texPath);
    }
}

Ground::~Ground() {
    if (texId) {
        glDeleteTextures(1, &texId);
    }
}

GLuint Ground::loadTexture(const char* path) {
    int w, h, n;
    unsigned char* data = stbi_load(path, &w, &h, &n, 0);
    if (!data) return 0;

    GLenum fmt = (n == 4) ? GL_RGBA : GL_RGB;
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, w, h, fmt, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

void Ground::draw(float size) const {
    if (!texId) return;

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);

    const float tiling = size / 0.8f; // repeat texture across ground

    glBegin(GL_QUADS);
    glTexCoord2f(0.f,        0.f);        glVertex3f(-size, -size, 0.f);
    glTexCoord2f(tiling,     0.f);        glVertex3f( size, -size, 0.f);
    glTexCoord2f(tiling,  tiling);        glVertex3f( size,  size, 0.f);
    glTexCoord2f(0.f,     tiling);        glVertex3f(-size,  size, 0.f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}
