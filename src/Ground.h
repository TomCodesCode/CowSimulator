#pragma once
#include <GL/glut.h>

class Ground {
public:
    Ground(const char* texPath = "src/grass.png");
    ~Ground();

    void draw(float size = 100.f) const;

private:
    GLuint texId = 0;

    GLuint loadTexture(const char* path);
};
    