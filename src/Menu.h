#pragma once
#include <GL/glut.h>

class Menu {
public:
    Menu() = default;

    // must be called from your reshape(w,h)
    void reshape(int w, int h);

    void draw();

    // forward raw mouse & motion events from main
    // returns true if the event was consumed by the menu (so main should NOT handle camera)
    bool mouse(int button, int state, int x, int y);
    bool motion(int x, int y);

    float ambient() const { return ambLevel; }
    void  setAmbient(float a) { ambLevel = (a<0?0:(a>1?1:a)); }

    // signal to main that user clicked "Quit"
    bool requestQuit = false;

private:
    // state
    bool menuOpen = false;
    bool draggingAmb = false;
    bool showHelp = false;
    float ambLevel = 0.25f;  // 0..1

    // viewport size
    int winW = 0, winH = 0;

    // rectangles
    struct Rect { float x,y,w,h; };
    Rect rMenuBtn{}, rPanel{}, rHelpBtn{}, rQuitBtn{}, rAmbLabel{}, rAmbTrack{}, rAmbKnob{};

    // helpers
    static bool ptInRect(int x, int y, const Rect& r) {
        return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
    }
    static void drawFilledRect(const Rect& r);
    static void drawString(float x, float y, const char* s);
    // compute rects from winW/winH
    void layout();
    void drawOverlay2D();
};
