#include "Texture.h"

#include <iostream>
#include <fstream>

#include <QColor>

using namespace std;

/********** TEXTURE *****************/

Texture::Texture(string name):
    name(name)
{}
Texture::~Texture() {}

Vec3Df Texture::getRepresentativeColor() const {
    return color;
}

void Texture::setRepresentativeColor(Vec3Df c) {
    color = c;
}

string Texture::getName() const {
    return name;
}

/********** MAPPED TEXTURE **********/

MappedTexture::MappedTexture(string name):
    Texture(name),
    uMax(1),
    vMax(1)
{}
MappedTexture::~MappedTexture() {}

Vec3Df MappedTexture::getColor(Ray *intersectingRay) const {
    if (!intersectingRay->intersect()) {
        cerr<<__FUNCTION__<<": cannot get the texture color!"<<endl;
        // Horrible pink for debug
        return Vec3Df(1, 0, 1);
    }

    const Triangle *t = intersectingRay->getTriangle();

    // TODO implement Vec2D...
    float uA = t->getU(0);
    float vA = t->getV(0);
    float uB = t->getU(1);
    float vB = t->getV(1);
    float uC = t->getU(2);
    float vC = t->getV(2);

    float interCA = intersectingRay->getU();
    float interCB = intersectingRay->getV();

    float uCA = uA - uC;
    float vCA = vA - vC;
    float uCB = uB - uC;
    float vCB = vB - vC;

    float interU = uC + uCA * interCA + uCB * interCB;
    float interV = vC + vCA * interCA + vCB * interCB;

    interU -= (int)(interU/uMax)*uMax;
    interU /= uMax;

    interV -= (int)(interV/vMax)*vMax;
    interV /= vMax;

    // Call abstract method
    Vec3Df color = getColor(interU, interV);
    return color;
}

/********** IMAGE TEXTURE ***********/

ImageTexture::ImageTexture(const char *fileName, string name):
    MappedTexture(name),
    image(nullptr)
{
    color = Vec3Df(1, 0, 1);
    image = new QImage(fileName);
    if (!image) {
        cerr<<__FUNCTION__<<": cannot read image "<<fileName<<endl;
    }
}

ImageTexture::~ImageTexture()
{
    delete image;
}

Vec3Df ImageTexture::getColor(float x, float y) const{
    

    unsigned int width = image->width();
    unsigned int height = image->height();
    unsigned int u = x * width;
    if (u == width) {
        u = width - 1;
    }
    unsigned int v = y * height;
    if (v == height) {
        v = height - 1;
    }
    QColor pixel = QColor(image->pixel(u, v));

    return Vec3Df(pixel.red(), pixel.green(), pixel.blue())/255.0;
}

/*********** BASIC TEXTURE ***********/

BasicTexture::BasicTexture(string name):
    MappedTexture(name)
{}

BasicTexture::~BasicTexture() {}

Vec3Df BasicTexture::getColor(float x, float y) const {
    bool isXEven = x*2.0<1.0;
    bool isYEven = y*2.0<1.0;

    if (isXEven==isYEven) {
        return Vec3Df(1, 0, 0);
    }
    return Vec3Df(0, 0, 1);
}

/************ COLOR TEXTURE ***********/

ColorTexture::ColorTexture(Vec3Df color, string name):
    Texture(name)
{
    this->color = color;
    if (name.empty()) {
        name = "Color Texture "+color.toString();
    }
}

ColorTexture::~ColorTexture()
{}

Vec3Df ColorTexture::getColor(Ray *) const {
    return color;
}

/******** NOISE TEXTURE **************/

NoiseTexture::NoiseTexture(Vec3Df color, float (*noise)(const Vertex &), string name):
    ColorTexture(color, name),
    noise(noise)
{}

NoiseTexture::~NoiseTexture() {}

Vec3Df NoiseTexture::getColor(Ray *ray) const {
    const Vertex &v = ray->getIntersection();
    return color*noise(v);
}
