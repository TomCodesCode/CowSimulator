#include "Material.h"

class House {
public:
    House();
    void draw() const;

private:
    void drawWalls() const;
    void drawRoof() const;
    void drawDoor() const;
    void drawWindows() const;

    // Material properties
    Material wallMaterial;
    Material roofMaterial;
    Material doorMaterial;
    Material windowMaterial;
};
