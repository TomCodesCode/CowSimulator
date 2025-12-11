#pragma once
#include <GL/glut.h>

struct Material {
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;

    // apply to current GL state
    void apply() const {
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }

    // make a material
    static Material make(GLfloat r,GLfloat g,GLfloat b,
                         GLfloat rs,GLfloat gs,GLfloat bs,
                         GLfloat sh) {
        return {{r,g,b,1.f}, {rs,gs,bs,1.f}, sh};
    }
    static Material Default() { return make(0.85f,0.85f,0.85f, 0.2f,0.2f,0.2f, 16.f); }
    static Material Pink (GLfloat sh=8.f)  { return make(1.0f,0.6f,0.75f, 0.1f,0.1f,0.1f, sh); }
    static Material Black(GLfloat sh=4.f)  { return make(0.f,0.f,0.f,     0.02f,0.02f,0.02f, sh); }
    static Material Green(GLfloat sh=8.f)  { return make(0.25f,0.7f,0.3f, 0.1f,0.1f,0.1f, sh); }
    static Material Brown(GLfloat sh=12.f) { return make(0.4f,0.25f,0.1f, 0.1f,0.1f,0.1f, sh); }
    static Material Red  (GLfloat sh=8.f)  { return make(0.9f,0.2f,0.1f,  0.1f,0.f,0.f, sh); }
    static Material White(GLfloat sh=4.f)  { return make(0.9f,0.9f,0.9f,  0.5f,0.5f,0.5f, sh); }
    static Material Yellow(GLfloat sh=8.f)  { return make(0.9f,0.9f,0.1f, 0.5f,0.5f,0.f, sh); }
    static Material Silver(GLfloat sh=100.f) { return make(0.4f,0.4f,0.4f, 1.f,1.f,1.f,sh);}
} typedef Material;

// restore previous material automatically
struct MaterialScope {
    MaterialScope(const Material& m) { glPushAttrib(GL_LIGHTING_BIT); m.apply(); }
    ~MaterialScope() { glPopAttrib(); }
} typedef MaterialScope;
