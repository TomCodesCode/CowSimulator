#include "Cow.h"
#include "Material.h"
#include <cmath>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "stb_image.h"

GLuint Cow::LoadTexture2D(const char* path, bool repeat) {
    int w=0, h=0, n=0;
    unsigned char* data = stbi_load(path, &w, &h, &n, 0);
    if (!data) {
        std::fprintf(stderr, "[Cow] Failed to load image: %s\n", path);
        return 0;
    }
    GLenum fmt = (n == 4) ? GL_RGBA : GL_RGB;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLint wrap = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // build mipmaps
    if (gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, w, h, fmt, GL_UNSIGNED_BYTE, data) != 0) {
        std::fprintf(stderr, "[Cow] gluBuild2DMipmaps() failed for %s\n", path);
        glDeleteTextures(1, &tex);
        tex = 0;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return tex;
}

// generic texture loading
void Cow::ensureTexturesLoaded() const {
    if (texturesReady) return;

    // Make lighting modulate textures (safe to call once here)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    texChecker = LoadTexture2D(checkerPath, /*repeat*/true);
    if (!texChecker) {
        std::fprintf(stderr, "[Cow] Using untextured fallback (no checker loaded)\n");
    }
    texturesReady = true;
}

// checker texture setup
void Cow::beginChecker(float tileS, float tileT) const {
    if (!texChecker) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texChecker);
    // simple tiling via texture matrix
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glScalef(tileS, tileT, 1.f);
    glMatrixMode(GL_MODELVIEW);
}

// end checker texture
void Cow::endChecker() const {
    if (!texChecker) return;
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

// helper for cylinder caps
static void drawDisk(float radius, int slices) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, 0.0, radius, slices, 1);
    gluDeleteQuadric(q);
}

// Cow implementation

Cow::Cow()
: headYaw(0.f), headPitch(0.f), tailYaw(0.f) {}

void Cow::clampAngles() {
    if (headYaw >  60.f) headYaw =  60.f;
    if (headYaw < -60.f) headYaw = -60.f;
    if (headPitch >  25.f) headPitch =  25.f;
    if (headPitch < -25.f) headPitch = -25.f;
    if (tailPitch >  90.f) tailPitch =  90.f;
    if (tailPitch < -20.f) tailPitch = -20.f;
    if (tailYaw >  80.f) tailYaw =  80.f;
    if (tailYaw < -80.f) tailYaw = -80.f;
}

// draw a cylinder
void Cow::drawUnitCylinder(float height, float radius, int slices, int stacks) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    // Cylinder along +Z
    gluCylinder(q, radius, radius, height, slices, stacks);

    // Caps
    glPushMatrix();
    drawDisk(radius, slices);
    glTranslatef(0.f, 0.f, height);
    glRotatef(180.f, 1.f, 0.f, 0.f);
    drawDisk(radius, slices);
    glPopMatrix();

    gluDeleteQuadric(q);
}

// draw a box. supports texture coords
void Cow::drawUnitBox(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);

    glBegin(GL_QUADS);
    // +X
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f( 0.5f,-0.5f,-0.5f);
    glTexCoord2f(1,0); glVertex3f( 0.5f, 0.5f,-0.5f);
    glTexCoord2f(1,1); glVertex3f( 0.5f, 0.5f, 0.5f);
    glTexCoord2f(0,1); glVertex3f( 0.5f,-0.5f, 0.5f);

    // -X
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f,-0.5f);
    glTexCoord2f(1,0); glVertex3f(-0.5f,-0.5f, 0.5f);
    glTexCoord2f(1,1); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0,1); glVertex3f(-0.5f, 0.5f,-0.5f);

    // +Y
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-0.5f, 0.5f,-0.5f);
    glTexCoord2f(1,0); glVertex3f( 0.5f, 0.5f,-0.5f);
    glTexCoord2f(1,1); glVertex3f( 0.5f, 0.5f, 0.5f);
    glTexCoord2f(0,1); glVertex3f(-0.5f, 0.5f, 0.5f);

    // -Y
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f,-0.5f);
    glTexCoord2f(1,0); glVertex3f(-0.5f,-0.5f, 0.5f);
    glTexCoord2f(1,1); glVertex3f( 0.5f,-0.5f, 0.5f);
    glTexCoord2f(0,1); glVertex3f( 0.5f,-0.5f,-0.5f);

    // +Z
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f, 0.5f);
    glTexCoord2f(1,0); glVertex3f( 0.5f,-0.5f, 0.5f);
    glTexCoord2f(1,1); glVertex3f( 0.5f, 0.5f, 0.5f);
    glTexCoord2f(0,1); glVertex3f(-0.5f, 0.5f, 0.5f);

    // -Z
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f(-0.5f,-0.5f,-0.5f);
    glTexCoord2f(1,0); glVertex3f( 0.5f,-0.5f,-0.5f);
    glTexCoord2f(1,1); glVertex3f( 0.5f, 0.5f,-0.5f);
    glTexCoord2f(0,1); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();

    glPopMatrix();
}


// draw a leg
void Cow::drawLeg(float length, float radius) {
    // draw cylinder along +Z, then move its base to origin
    glPushMatrix();
    drawUnitCylinder(length, radius);
    glPopMatrix();
}

void Cow::drawTailSegment(float length, float radius) {
    drawUnitCylinder(length, radius, 16, 1);
}

void Cow::drawEar(float w, float h, float d) {
    drawUnitBox(w, h, d);
}

// main draw function (called from main)
void Cow::draw() const {
    // Basic material (light gray/white cow)
    Material mat = Material::Default();
    mat.apply();

    ensureTexturesLoaded();

    glPushMatrix();

    glTranslatef(dx, dy, 0.f); // move to current position (walking)
    
    if (walking == Forward)  glRotatef(0.f,   0.f,0.f,1.f);
    if (walking == Backward) glRotatef(180.f, 0.f,0.f,1.f);
    if (walking == Left)     glRotatef(90.f,  0.f,0.f,1.f);
    if (walking == Right)    glRotatef(-90.f, 0.f,0.f,1.f);

    // BODY (a stretched box)
    const float bodyL = 1.6f, bodyW = 1.0f, bodyH = 0.8f;
    glPushMatrix();
    glTranslatef(0.f, 0.f, 1.2f); // lift above ground so legs reach z=0
    beginChecker(1.f, 1.f);
    drawUnitBox(bodyL, bodyW, bodyH);
    endChecker();
    glPopMatrix();

    // HEAD (sphere + snout/nose box), attached to front of body
    glPushMatrix();
    {
        // position at front-top of body
        glTranslatef(bodyL*0.45f, 0.f, 0.6f + bodyH*0.9f);
        
        // head pivot
        glRotatef(headYaw,   0.f, 0.f, 1.f); // yaw: left/right (around Z)
        glRotatef(headPitch, 0.f, 1.f, 0.f); // pitch: up/down  (around Y)

        // Neck (a short cylinder)
        beginChecker(1.f, 1.f);
        glPushMatrix();
        glTranslatef(-0.1f, 0.f, 0.f);
        glRotatef(90.f, 0.f, 1.f, 0.f);
        drawUnitBox(0.4f, 0.5f, 0.5f);
        glPopMatrix();
        
        // Skull
        glPushMatrix();
        glTranslatef(0.3f, 0.f, 0.05f);
        //glutSolidSphere(0.35, 24, 16);
        drawUnitBox(0.4f, 0.6f, 0.6f);
        glPopMatrix();
        endChecker();

        // Snout (small box)
        {
            MaterialScope mscope( Material::Pink(8.f) );
            glPushMatrix();
            glTranslatef(0.5f, 0.f, -0.1f);
            drawUnitBox(0.25f, 0.25f, 0.25f);
            glPopMatrix();
        }

        // Ears (thin boxes)
        beginChecker(0.1f, 0.2f);
        glPushMatrix();
        glTranslatef(0.35f,  0.33f, 0.3f);
        drawEar(0.1f, 0.1f, 0.3f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.35f, -0.33f, 0.3f);
        drawEar(0.1f, 0.1f, 0.3f);
        glPopMatrix();
        endChecker();

        // Eyes
        {
            MaterialScope mscope( Material::Black(50.f) );
            glPushMatrix();
            glTranslatef(0.5f,  0.18f, 0.15f);
            glScalef(0.5f, 1.2f, 0.7f);
            glutSolidSphere(0.07, 12, 12);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0.5f, -0.18f, 0.15f);
            glScalef(0.5f, 1.2f, 0.7f);
            glutSolidSphere(0.07, 12, 12);
            glPopMatrix();
        }
    }
    glPopMatrix();

    // TAIL (two segments), attached to rear of body
    glPushMatrix();
    {
        glTranslatef(-bodyL*0.495f, 0.f, 0.6f + bodyH*1.2f);
        glRotatef(tailYaw, 0.f, 0.f, 1.f);      // wag around local Z
        glRotatef(tailPitch, 0.f, 1.f, 0.f);    // user pitch around local Y
        glRotatef(-90.f, 0.f, 1.f, 0.f);        // build along +Z outward from body side
        glRotatef(-60.f, 0.f, 1.f, 0.f);        // initial downward tilt

        // Segment 1
        beginChecker(0.1f, 0.4f);
        glTranslatef(-0.1f, 0.f, 0.6f);
        drawUnitBox(0.1f, 0.1f, 1.3f);
        endChecker();
        
        // Segment 2 (tip)
        MaterialScope mscope( Material::Black(4.f) );
        glTranslatef(0.f, 0.f, 0.65f);
        glRotatef(90.f, 0.f, 1.f, 0.f);
        glScalef(2.f, 0.9f, 0.9f);
        glutSolidSphere(0.1, 10, 10);
    }
    glPopMatrix();

    // LEGS
    const float legLen = 1.f, legW = 0.3f;
    const float xOff = bodyL*0.35f, yOff = bodyW*0.35f;
    float now = 0.001f * glutGet(GLUT_ELAPSED_TIME);
    float legAngle = 0.f;
    if (walkingEnabled) {
        const float stepsPerSecond = 2.f;
        float phase = 2.f * M_PI * stepsPerSecond * now;
        legAngle = std::sin(phase) * 30.f;  // 30 degrees movement
    }

    auto placeLeg = [&](float x, float y, float angle) {
        glPushMatrix();
        const float hipZ = 1.f; // top of the hip joint
        glTranslatef(x, y, hipZ);
        glRotatef(angle, 0.f, 1.f, 0.f);         // swing around Y
        glTranslatef(0.f, 0.f, -legLen*0.5f);
        beginChecker(0.4f, 0.6f);
        drawUnitBox(legW, legW, legLen);
        endChecker();
        glPopMatrix();
    };

    // move legs up so they attach to the body bottom
    glPushMatrix();
    glTranslatef(0.f, 0.f, 0.0f); // bases at ground already

    placeLeg( xOff,  yOff, -legAngle); // front right
    placeLeg( xOff, -yOff, legAngle); // front left
    placeLeg(-xOff,  yOff, legAngle); // rear  right
    placeLeg(-xOff, -yOff, -legAngle); // rear  left
    glPopMatrix();

    glPopMatrix(); // cow root
}

void Cow::walk(WalkingDir dir) {
    walkingEnabled = true;
    walking = dir;
    switch (walking) {
        case Forward:  // move forward
            dx += 0.1f;
            break;
        case Backward: // move backward
            dx -= 0.1f;
            break;
        case Left:     // turn left
            dy += 0.1f;
            break;
        case Right:    // turn right
            dy -= 0.1f;
            break;
    }
}