#pragma once
#include <GL/glut.h>

class Cow {
public:
    Cow();

    // position changes (walking)
    float dx = 0, dy = 0; // ground plane

    enum WalkingDir { Forward, Backward, Left, Right };
    void setWalking(WalkingDir dir) {
        walkingEnabled = true;
        walking = dir;

        // if the cow turns, rotate body to face that way
        switch (dir) {
            case Forward:  bodyYawDeg =   0.f;  break;
            case Backward: bodyYawDeg = 180.f;  break;
            case Left:     bodyYawDeg =  90.f;  break;
            case Right:    bodyYawDeg = -90.f;  break;
        }

        walk(dir); // keep your existing nudge if you still want it
    }


    // draw at the origin
    void draw() const;

    void updateWalk(float dt); // update animation by dt seconds
    bool isWalking() const { return walkingEnabled; }
    void setWalkingEnabled(bool enabled) { walkingEnabled = enabled; }

    // body control
    float getBodyYaw() const { return bodyYawDeg; }

    // head control
    void addHeadYaw(float d)   { headYaw += d; clampAngles(); }
    void addHeadPitch(float d) { headPitch -= d; clampAngles(); }

    // tail control
    void addTailYaw(float d) { tailYaw += d; clampAngles(); }
    void addTailPitch(float d) { tailPitch -= d; clampAngles(); }

    float getHeadYaw() const { return headYaw; }
    float getHeadPitch() const { return headPitch; }

private:
    // state
    float headYaw;     // left/right
    float headPitch;   // up/down
    float tailYaw;     // current tail yaw
    float tailPitch;   // current tail pitch

    // body
    float bodyYawDeg = 0.f;
    
    // walking
    mutable bool walkingEnabled = false;
    WalkingDir walking;
    void walk(WalkingDir dir);

    mutable bool   texturesReady = false;
    mutable GLuint texChecker    = 0;
    const char*    checkerPath   = "src/checkers.png";

    void ensureTexturesLoaded() const;
    void beginChecker(float tileS = 1.f, float tileT = 1.f) const;
    void endChecker() const;
    static GLuint LoadTexture2D(const char* path, bool repeat);

    // helpers
    void clampAngles();
    static void drawUnitCylinder(float height, float radius, int slices = 24, int stacks = 1);
    static void drawUnitBox(float x=1.f, float y=1.f, float z=1.f);
    static void drawLeg(float length, float radius);
    static void drawTailSegment(float length, float radius);
    static void drawEar(float w, float h, float d);
};
