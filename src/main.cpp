#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cctype>
#include "Cow.h"
#include "Tree.h"
#include "House.h"
#include "Ground.h"
#include "Menu.h"
#include "Light.h"

static Cow cow;
static Menu menu;
static Light userControlledLight;

// default window size
static int winW = 1280, winH = 720;

// simple orbit camera
static float camAzim = 25.f;   // degrees around Z
static float camElev = 20.f;   // degrees above XY plane
static float camDist = 6.5f;   // distance from origin
static bool firstPerson = false; // if true, looking through cow's eyes

// mouse tracking
static int lastX = 0, lastY = 0;
static bool rotating = false;
static float targetX = 0.f, targetY = 0.f, targetZ = 0.5f; // what we look at
static bool  panning = false;  // right-button drag

// zoom
static const float kZoomMin = 2.0f;
static const float kZoomMax = 20.0f;
static const float kZoomStep = 0.4f;

static float clamp(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

// apply zoom change and clamp
static void applyZoom(float delta) {
    camDist = clamp(camDist + delta, kZoomMin, kZoomMax);
    glutPostRedisplay();
}

// general lighting
static void setLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat pos[] = { 4.f, 3.f, 6.f, 1.f };
    GLfloat spec[] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat diff[] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat amb [] = { menu.ambient(), menu.ambient(), menu.ambient(), 1.f };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
}

// simple ground plane for reference
static void drawGround() {
    glDisable(GL_LIGHTING);
    glColor3f(0.7f, 0.75f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-100.f, -100.f, 0.f);
    glVertex3f( 100.f, -100.f, 0.f);
    glVertex3f( 100.f,  100.f, 0.f);
    glVertex3f(-100.f,  100.f, 0.f);
    glEnd();
    glEnable(GL_LIGHTING);
}

// handle window resize
static void reshape(int w, int h) {
    winW = w; winH = h > 1 ? h : 1;
    glViewport(0, 0, winW, winH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)winW / (double)winH, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    menu.reshape(w, h);
}

// main display
static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera placement (spherical coords)
    if (!firstPerson){
        const float az = camAzim * 3.14159265f / 180.f;
        const float el = camElev * 3.14159265f / 180.f;
        float cx = camDist * std::cos(el) * std::cos(az);
        float cy = camDist * std::cos(el) * std::sin(az);
        float cz = camDist * std::sin(el);
        gluLookAt(cx + targetX, cy + targetY, cz + targetZ, targetX, targetY, targetZ, 0, 0, 1);
    } else {
        const float eyeOffX = 1.52f;   // head pivot + eyes forward
        const float eyeOffY = 0.00f;   // centered between eyes
        const float eyeOffZ = 1.52f;   // head height

        // Use the cow's head yaw/pitch to aim camera
        const float bodyYaw = cow.getBodyYaw() * 3.14159265f / 180.f;   // body's rotation
        const float headYaw = cow.getHeadYaw() * 3.14159265f / 180.f;
        const float pitch   = cow.getHeadPitch() * 3.14159265f / 180.f;

        const float yaw = bodyYaw + headYaw; // combined body + head yaw

        const float dirx = std::cos(pitch) * std::cos(yaw);
        const float diry = std::cos(pitch) * std::sin(yaw);
        const float dirz = -std::sin(pitch);

        // Eye in world
        const float eyeX = cow.dx + eyeOffX * std::cos(bodyYaw) - eyeOffY * std::sin(bodyYaw);
        const float eyeY = cow.dy + eyeOffX * std::sin(bodyYaw) + eyeOffY * std::cos(bodyYaw);
        const float eyeZ = eyeOffZ;

        // further out to avoid clipping
        const float lookDist = 0.1f;
        const float ctrX = eyeX + dirx * lookDist;
        const float ctrY = eyeY + diry * lookDist;
        const float ctrZ = eyeZ + dirz * lookDist;

        // Keep up as +Z
        gluLookAt(eyeX, eyeY, eyeZ,  ctrX, ctrY, ctrZ,  0, 0, 1);
    }

    
    setLighting();

    userControlledLight.apply();

    // Ground
    static Ground ground;
    ground.draw(100.f);

    // Draw house
    static House house;
    house.draw();

    // Draw tree
    static Tree tree1, tree2, tree3, tree4, tree5, tree6, tree7, tree8, tree9, tree10;
    tree1.draw(); tree2.draw(); tree3.draw(); tree4.draw(); tree5.draw();
    tree6.draw(); tree7.draw(); tree8.draw(); tree9.draw(); tree10.draw();

    // Draw cow at origin
    cow.draw();

    // Overlay menu
    menu.draw();

    glutSwapBuffers();

    if (cow.isWalking()) {
        glutPostRedisplay();
    }

    if (menu.requestQuit) {
        std::printf("No more moo for you!\n");
        std::fflush(stdout);
        std::exit(0);
    }
}

// handle key presses
static void keyboard(unsigned char key, int, int) {
    switch (tolower(key)) {
        case 27: // ESC
            std::printf("No more moo for you!\n");
            std::fflush(stdout);
            std::exit(0);
            break;
        case 32:
            firstPerson = !firstPerson;
            break;

        // head control
        case 'j': cow.addHeadYaw(+5.f);   break;
        case 'l': cow.addHeadYaw(-5.f);   break;
        case 'i': cow.addHeadPitch(+5.f); break;
        case 'k': cow.addHeadPitch(-5.f); break;

        // tail control
        case '1':
            cow.addTailPitch(+5.f);
            break;
        case '2':
            cow.addTailPitch(-5.f);
            break;
        case '3':
            cow.addTailYaw(+5.f);
            break;
        case '4':
            cow.addTailYaw(-5.f);
            break;

        case 'q': // increase user light intensity
            for (int i = 0; i < 3; i++){
                userControlledLight.diff[i] = std::min(userControlledLight.diff[i] + 0.05f, 1.f);
                userControlledLight.spec[i] = std::min(userControlledLight.spec[i] + 0.05f, 1.f);
            }
            break;
        case 'e': // decrease user light intensity
            for (int i = 0; i < 3; i++){
                userControlledLight.diff[i] = std::max(userControlledLight.diff[i] - 0.05f, 0.f);
                userControlledLight.spec[i] = std::max(userControlledLight.spec[i] - 0.05f, 0.f);
            }
            break;

        case 'w': // move user light
            userControlledLight.pos[0] -= 0.05f;
            break;
        case 's':
            userControlledLight.pos[0] += 0.05f;
            break;
        case 'a':
            userControlledLight.pos[1] -= 0.05f;
            break;
        case 'd':
            userControlledLight.pos[1] += 0.05f;
            break;
        case 'x':
            userControlledLight.pos[2] -= 0.05f;
            break;
        case 'z':
            userControlledLight.pos[2] += 0.05f;
            break;
        case 'r': // rotate user light
            userControlledLight.rotate(2.f, 0.f);
            break;
        case 'f':
            userControlledLight.rotate(-2.f, 0.f);
            break;
        case 'v':
            userControlledLight.rotate(0.f, 2.f);
            break;
        case 'c':
            userControlledLight.rotate(0.f, -2.f);
            break;
        default: break;
    }
    glutPostRedisplay();
}

// start walking when arrow key pressed
static void special(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:
            cow.setWalking(Cow::Forward);
            break;
        case GLUT_KEY_DOWN:
            cow.setWalking(Cow::Backward);
            break;
        case GLUT_KEY_LEFT:
            cow.setWalking(Cow::Left);
            break;
        case GLUT_KEY_RIGHT:
            cow.setWalking(Cow::Right);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

// stop walking when arrow key released
static void specialUp(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            cow.setWalkingEnabled(false);
            break;
    }
    glutPostRedisplay();
}

// mouse handling for camera control
static void mouse(int button, int state, int x, int y) {
    if (menu.mouse(button, state, x, y)) { glutPostRedisplay(); return; }
    
    if (button == GLUT_LEFT_BUTTON) {
        rotating = (state == GLUT_DOWN);
        lastX = x; lastY = y;
    }

    if (button == GLUT_RIGHT_BUTTON) {
        panning = (state == GLUT_DOWN);
        if (panning) { lastX = x; lastY = y; }
        return;
    }

    if (state == GLUT_DOWN) {
        if (button == 3) {           // scroll up = zoom in
            applyZoom(-kZoomStep);
        } else if (button == 4) {    // scroll down = zoom out
            applyZoom(+kZoomStep);
        }
    }
}

static void motion(int x, int y) {
    if (menu.motion(x, y)) { glutPostRedisplay(); return; }

    int dx = x - lastX;
    int dy = y - lastY;
    lastX = x; lastY = y;

    if (rotating) {
        camAzim += dx * 0.4f;
        camElev += dy * 0.3f;
        if (camElev > 85.f)  camElev = 85.f;
        if (camElev < -10.f) camElev = -10.f;
        glutPostRedisplay();
        return;
    }

    if (panning) {
        float panSpeed = camDist * 0.002f;

        // camera azimuth in radians
        const float az = camAzim * 3.14159265f / 180.f;

        // screen-right vector
        const float rx = -std::sin(az);
        const float ry =  std::cos(az);

        // screen-up vector
        const float ux = -std::cos(az);
        const float uy = -std::sin(az);
        
        targetX += (-dx * rx + dy * ux) * panSpeed;
        targetY += (-dx * ry + dy * uy) * panSpeed;

        glutPostRedisplay();
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Tom's Farm- a Cow Simulation");

    glEnable(GL_DEPTH_TEST);    // z-buffer
    glEnable(GL_NORMALIZE);     // normalize
    glShadeModel(GL_SMOOTH);
    glClearColor(0.3f, 0.7f, 0.98f, 1.0f);

    reshape(winW, winH);
    userControlledLight.setDirectional(45.f, 45.f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
