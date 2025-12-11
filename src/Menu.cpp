#include "Menu.h"

// filled rect
void Menu::drawFilledRect(const Rect& r) {
    glBegin(GL_QUADS);
    glVertex2f(r.x,       r.y); // bottom-left
    glVertex2f(r.x+r.w,   r.y); // bottom-right
    glVertex2f(r.x+r.w,   r.y+r.h); // top-right
    glVertex2f(r.x,       r.y+r.h); // top-left
    glEnd();
}

// bitmap text
void Menu::drawString(float x, float y, const char* s) {
    glRasterPos2f(x, y);
    while (*s) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++);
}

// fit rectangles to current window size
void Menu::layout() {
    rMenuBtn = { 12.f, (float)winH - 45.f, 45.f, 30.f };
    rPanel   = { 12.f, (float)winH - 45.f - 160.f, 220.f, 150.f };

    float px = rPanel.x + 12.f, py = rPanel.y + rPanel.h - 40.f;
    rHelpBtn = { px, py, 196.f, 24.f };           py -= 32.f;

    rAmbLabel = { px, py, 60.f, 20.f };
    rAmbTrack = { px + 70.f, py+8.f, 120.f, 4.f }; py -= 32.f;

    rQuitBtn = { px, rPanel.y + 12.f, 196.f, 24.f };
}

// 2D overlay
void Menu::drawOverlay2D() {
    // panel & widgets
    // menu button
    glColor3f(0.1f,0.1f,0.1f); drawFilledRect(rMenuBtn);
    glColor3f(0.95f,0.95f,0.95f); drawString(rMenuBtn.x + 7, rMenuBtn.y + 9, "MENU");

    if (menuOpen) {
        // panel
        glColor3f(0.f,0.f,0.f); drawFilledRect({rPanel.x-1,rPanel.y-1,rPanel.w+2,rPanel.h+2});
        glColor3f(0.12f,0.12f,0.12f); drawFilledRect(rPanel);

        // Help
        glColor3f(0.25f,0.25f,0.25f); drawFilledRect(rHelpBtn);
        glColor3f(1,1,1); drawString(rHelpBtn.x+8, rHelpBtn.y+7, "Help");

        // Ambient slider
        glColor3f(1,1,1); drawString(rAmbLabel.x, rAmbLabel.y+4, "Ambient");
        glColor3f(0.3f,0.3f,0.3f); drawFilledRect(rAmbTrack);
        float kx = rAmbTrack.x + ambLevel * rAmbTrack.w - 6.f;
        rAmbKnob = { kx, rAmbTrack.y - 6.f, 12.f, 16.f };
        glColor3f(0.8f,0.8f,0.8f); drawFilledRect(rAmbKnob);

        // Quit
        glColor3f(0.35f,0.1f,0.1f); drawFilledRect(rQuitBtn);
        glColor3f(1,1,1); drawString(rQuitBtn.x+8, rQuitBtn.y+7, "Quit");
    }

    // help
    if (showHelp) {
        Rect r = { (float)winW-520.f, (float)winH-300.f, 500.f, 280.f };
        glColor3f(0.f,0.f,0.f); drawFilledRect({r.x-1,r.y-1,r.w+2,r.h+2});
        glColor3f(0.08f,0.08f,0.08f); drawFilledRect(r);
        glColor3f(1,1,1);
        float y = r.y + r.h - 24.f; // start near top
        drawString(r.x+10, y, "Controls:");
        y -= 18; drawString(r.x+10, y, "Left-drag: rotate");
        y -= 18; drawString(r.x+10, y, "Right-drag: pan");
        y -= 18; drawString(r.x+10, y, "Scroll: zoom");
        y -= 18; drawString(r.x+10, y, "Space: toggle first/third person view");
        y -= 18; drawString(r.x+10, y, "i/k: head pitch  |  j/l: head yaw");
        y -= 18; drawString(r.x+10, y, "1/2/3/4: tail pitch/yaw");
        y -= 18; drawString(r.x+10, y, "Arrow keys: walk");
        y -= 18; drawString(r.x+10, y, "w/s/a/d/x/z: move light");
        y -= 18; drawString(r.x+10, y, "r/f: rotate light");
        y -= 18; drawString(r.x+10, y, "v/c: tilt light");
        y -= 18; drawString(r.x+10, y, "q/e: increase/decrease light intensity");
        y -= 18; drawString(r.x+10, y, "Click Help again to hide");
    }
}

// change viewport size. override default
void Menu::reshape(int w, int h) {
    winW = w; winH = (h>1? h:1);
    layout();
}

void Menu::draw() {
    // Save states explicitly
    GLboolean wasDepthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean wasLighting  = glIsEnabled(GL_LIGHTING);
    GLboolean wasTex2D     = glIsEnabled(GL_TEXTURE_2D);

    GLint prevMatrixMode; glGetIntegerv(GL_MATRIX_MODE, &prevMatrixMode);

    // Save matrices
    glMatrixMode(GL_PROJECTION); glPushMatrix();
    glMatrixMode(GL_MODELVIEW);  glPushMatrix();

    // 2D overlay setup
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

    // Draw overlay
    drawOverlay2D();

    // Restore matrices
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();

    // Restore enables
    if (wasDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (wasLighting)  glEnable(GL_LIGHTING);   else glDisable(GL_LIGHTING);
    if (wasTex2D)     glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);

    glMatrixMode(prevMatrixMode);
}


bool Menu::mouse(int button, int state, int x, int y) {
    const int gy = winH - y; // convert to bottom-left origin

    // left press
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // toggle menu
        if (ptInRect(x, gy, rMenuBtn)) { menuOpen = !menuOpen; draggingAmb = false; return true; }

        if (menuOpen) {
            if (ptInRect(x, gy, rHelpBtn)) { showHelp = !showHelp; return true; }
            if (ptInRect(x, gy, rQuitBtn)) { requestQuit = true; return true; }
            if (ptInRect(x, gy, rAmbKnob) || ptInRect(x, gy, rAmbTrack)) {
                draggingAmb = true;
                float t = (x - rAmbTrack.x) / rAmbTrack.w;
                ambLevel = (t<0.f?0.f:(t>1.f?1.f:t));
                return true;
            }
            // clicking inside panel but not on a control still consumes
            if (ptInRect(x, gy, rPanel)) return true;
        }
    }
    // left release
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (draggingAmb) { draggingAmb = false; return true; }
    }
    // if menu is open, eat other left clicks
    return false;
}

// handle mouse motion. override default
bool Menu::motion(int x, int y) {
    if (!draggingAmb) return false;
    float t = (x - rAmbTrack.x) / rAmbTrack.w;
    ambLevel = (t<0.f?0.f:(t>1.f?1.f:t));
    return true;
}
