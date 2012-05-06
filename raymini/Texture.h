#pragma once

#include "Vec3D.h"

/** Load PPM */
class Texture
{
public:
    Texture();
    ~Texture();
    void loadPPM(const char *name);

    Vec3Df getColor(unsigned int x, unsigned int y) const;

    unsigned int getWidth() const {return width;}
    unsigned int getHeight() const {return height;}
private:
    unsigned int width,
                 height,
                 max;
    unsigned char *values;

};
