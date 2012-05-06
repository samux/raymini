#include "Texture.h"

#include <iostream>
#include <fstream>

using namespace std;

Texture::Texture():
    values(NULL)
{}

Texture::~Texture()
{
	if (values) {
		delete []values;
    }
}

void Texture::loadPPM(const char *name)
{
	string type;
	ifstream in(name);
	getline(in, type);
	getline(in, type);
	in >> width >> height >> max;
	if (max > 255) {
		cerr << "More than byte size is not supported." << endl;
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

Vec3Df Texture::getColor(float x, float y) const{
    unsigned int u = x * width;
    if (u == width) {
        u = width - 1;
    }
    unsigned int v = y * height;
    if (v == height) {
        v = height - 1;
    }
    unsigned int index = (u + v * width) * 3;

    return Vec3Df(values[index], values[index+1], values[index+2]);
}

BasicTexture::BasicTexture(): Texture() {}

BasicTexture::~BasicTexture() {}

#define SQUARE_WIDTH 20
Vec3Df BasicTexture::getColor(float x, float y) const {
    bool isXEven = (int)(x*100/SQUARE_WIDTH)%2 == 0;
    bool isYEven = (int)(y*100/SQUARE_WIDTH)%2 == 0;

    if (isXEven==isYEven) {
        return Vec3Df(255, 0, 0);
    }
    return Vec3Df(0, 0, 255);
}
