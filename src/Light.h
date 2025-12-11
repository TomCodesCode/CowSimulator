#pragma once
#include <GL/glut.h>
#include <cmath>

struct Light
{
    GLenum id = GL_LIGHT1; // GL_LIGHT0 is the main light, which is not user-controlled

    GLfloat pos[4] = { 1.f, 1.f, 1.f, 0.f }; // directional light because w=0
    GLfloat diff[4] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat spec[4] = { 1.f, 1.f, 1.f, 1.f };

    float azimDeg = 45.f; // degrees from +X towards +Y
    float elevDeg = 45.f; // degrees above XY plane

    void apply() const {
        glEnable(id);
        glLightfv(id, GL_POSITION, pos);
        glLightfv(id, GL_DIFFUSE,  diff);
        glLightfv(id, GL_SPECULAR, spec);
    }

    void setDirectional(float azDeg, float elDeg) {
        azimDeg = azDeg;
        elevDeg = elDeg < -89.f ? -89.f : elDeg > 89.f ? 89.f : elDeg;

        const float az = azimDeg * M_PI / 180.f; // radians
        const float el = elevDeg * M_PI / 180.f; // radians

        const float dx = std::cos(el) * std::cos(az); // direction vector
        const float dy = std::cos(el) * std::sin(az);
        const float dz = std::sin(el);

        pos[0] = dx; pos[1] = dy; pos[2] = dz; pos[3] = 0.f; // update position
    }

    // light rotation
    void rotate(float dazDeg, float delDeg) {
        setDirectional(azimDeg + dazDeg, elevDeg + delDeg);
    }
};
