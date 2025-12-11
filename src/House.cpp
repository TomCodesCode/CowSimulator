#include "House.h"
#include <GL/glut.h>

House::House()
    : wallMaterial(Material::Yellow(6.f)),
      roofMaterial(Material::Red(8.f)),
      doorMaterial(Material::Brown(8.f)),
      windowMaterial(Material::Silver(10000.f)) {}

// main draw function (called from main)
void House::draw() const {
    glPushMatrix();
    glTranslatef(-16.0f, 16.0f, 0.0f);
    glTranslatef(0.f, 0.f, 3.0f);
    glScalef(15.0f, 10.0f, 6.0f);
    drawWalls();
    drawRoof();
    drawDoor();
    drawWindows();
    glPopMatrix();
}

void House::drawWalls() const {
    MaterialScope mscope(wallMaterial);
    glPushMatrix();
    glutSolidCube(1.0f);
    glPopMatrix();
}

void House::drawRoof() const {
    MaterialScope mscope(roofMaterial);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.8f);
    glScalef(0.51f, 0.51f, 0.3f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
    glEnd();
    
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f,  1.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
    glEnd();
    glPopMatrix();
}

void House::drawDoor() const {
    MaterialScope mscope(doorMaterial);
    glPushMatrix();
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.5f, 0.0f, -0.2f);
    glScalef(0.02f, 0.125f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void House::drawWindows() const {
    MaterialScope mscope(windowMaterial);
    glPushMatrix();
    glTranslatef(0.5f, 0.0f, 0.0f);
    glScalef(0.02f, 0.2f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.5f, 0.0f, 0.0f);
    glScalef(0.02f, 0.2f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.5f, 0.3f, 0.0f);
    glScalef(0.02f, 0.2f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.5f, -0.3f, 0.0f);
    glScalef(0.02f, 0.2f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.5f, 0.3f, 0.0f);
    glScalef(0.02f, 0.2f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.5f, -0.3f, 0.0f);
    glScalef(0.02f, 0.2f, 0.4f);
    glutSolidCube(1.0f);
    glPopMatrix();
}
