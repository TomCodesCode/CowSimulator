#include "Tree.h"
#include <GL/glut.h>
#include <cstdlib>
#include "Material.h"

Tree::Tree() {
    posX = random_float(-15.0f, 15.0f);
    posY = random_float(-15.0f, 15.0f);
}

Tree::~Tree() {
    // Destructor implementation
}

float Tree::random_float(float min, float max) const {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void Tree::draw() const {
    glPushMatrix();
    glScalef(3.f, 3.f, 4.f);
    glTranslatef(posX, posY, 0.0f);
    drawBranch(2.0f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 2.0f);
    drawLeafs(0.5f);
    glPopMatrix();
    glPopMatrix();
}

void Tree::drawBranch(float length, float radius) const {
    MaterialScope mscope( Material::Brown(12.f) );
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    GLUquadric* quad = gluNewQuadric();
    glScalef(radius, radius, length);
    gluCylinder(quad, 1.0f, 1.0f, 1.0f, 8, 1);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void Tree::drawLeaf(float size) const {
    MaterialScope mscope( Material::Green(8.f) );
    glPushMatrix();
    glScalef(size, size, size);
    glutSolidSphere(1.0f, 8, 8);
    glPopMatrix();
}

void Tree::drawLeafs(float size) const {
    const float d = 1.f * size;  // spacing so spheres don't overlap

    glPushMatrix();
    // +X
    glPushMatrix(); glTranslatef( d, 0.f, 0.f); drawLeaf(size); glPopMatrix();
    // -X
    glPushMatrix(); glTranslatef(-d, 0.f, 0.f); drawLeaf(size); glPopMatrix();
    // +Y
    glPushMatrix(); glTranslatef(0.f,  d, 0.f); drawLeaf(size); glPopMatrix();
    // -Y
    glPushMatrix(); glTranslatef(0.f, -d, 0.f); drawLeaf(size); glPopMatrix();
    // higher (+Z)
    glPushMatrix(); glTranslatef(0.f, 0.f,  d); drawLeaf(size); glPopMatrix();
    glPopMatrix();
}
