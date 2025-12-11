#include <GL/glut.h>

class Tree {
    public:
        float posX, posY;

        Tree();
        ~Tree();

        void draw() const;

    private:
        void drawBranch(float length, float radius) const;
        void drawLeaf(float size) const;
        void drawLeafs(float size) const;
        float random_float(float min, float max) const;
};