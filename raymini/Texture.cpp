#include "Texture.h"

#include <iostream>
#include <fstream>

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
    Texture(name)
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

    // Call abstract method
    Vec3Df color = getColor(interU, interV);
    return color;
}

/********** PPM TEXTURE ***********/

PPMTexture::PPMTexture(const char *fileName, string name):
    MappedTexture(name),
    values(nullptr)
{
    loadPPM(fileName);
    setColorToMeanValue();
}

PPMTexture::~PPMTexture()
{
	if (values) {
		delete []values;
    }
}

void PPMTexture::loadPPM(const char *name)
{
	string type;
	ifstream in(name);
	getline(in, type);
	getline(in, type);
	in >> width >> height >> max;
	if (max > 255) {
		cerr << "More than byte size is not supported." << endl;
        return;
    }
	values = new unsigned char[width*height*3];
	for (unsigned int i=0; i<height*width*3; i++)
	{
		unsigned int j;
		in >> j;
		values[i] = j;
	}
	in.close();
}

Vec3Df PPMTexture::getColor(float x, float y) const{
    unsigned int u = x * width;
    if (u == width) {
        u = width - 1;
    }
    unsigned int v = y * height;
    if (v == height) {
        v = height - 1;
    }
    unsigned int index = (u + v * width) * 3;

    return Vec3Df(values[index], values[index+1], values[index+2])/255.0;
}

void PPMTexture::setColorToMeanValue() {
    Vec3Df mean;
    unsigned long count=0;
    const unsigned int widthTakenCount=200;
    const unsigned int heightTakenCount=100;
    for (unsigned int i=0; i<widthTakenCount; i++) {
        for (unsigned int j=0; j<heightTakenCount; j++) {
            mean += getColor((float)i/(float)widthTakenCount, (float)j/(float)heightTakenCount);
            count++;
        }
    }
    color = mean/count;
}

/*********** BASIC TEXTURE ***********/

BasicTexture::BasicTexture(string name):
    MappedTexture(name)
{}

BasicTexture::~BasicTexture() {}

#define SQUARE_WIDTH 20
Vec3Df BasicTexture::getColor(float x, float y) const {
    bool isXEven = (int)(x*100/SQUARE_WIDTH)%2 == 0;
    bool isYEven = (int)(y*100/SQUARE_WIDTH)%2 == 0;

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
