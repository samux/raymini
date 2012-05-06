#pragma once

#include "Vec3D.h"

/** Load PPM */
class Texture
{
public:
    Texture();
    virtual ~Texture();
    void loadPPM(const char *name);

    virtual Vec3Df getColor(float x, float y) const;

    unsigned int getWidth() const {return width;}
    unsigned int getHeight() const {return height;}
private:
    unsigned int width,
                 height,
                 max;
    unsigned char *values;

};

/** Basic debug texture, no memory space needed */
class BasicTexture: public Texture {
public:
    BasicTexture();
    virtual ~BasicTexture();

    virtual Vec3Df getColor(float x, float y) const;
};
