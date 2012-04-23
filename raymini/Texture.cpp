#include "Texture.h"

#include <iostream>
#include <fstream>

using namespace std;

Texture::Texture():
    v(NULL)
{}

Texture::~Texture()
{
	if (v)
		delete []v;
}

void Texture::loadPPM(const char *name)
{
	string type;
	ifstream in(name);
	getline(in, type);
	getline(in, type);
	in >> width >> height >> max;
	if (max > 255)
		cerr << "More than byte is not supported." << endl;
	v = new unsigned char[width*height*3];
	for (unsigned int i=0; i<height*width*3; i++)
	{
		unsigned int j;
		in >> j;
		v[i] = j;
	}
	in.close();
}

Vec3Df Texture::getColor(unsigned int x, unsigned int y) const{
    unsigned int index = (x + y * width) * 3;

    return Vec3Df(v[index], v[index+1], v[index+2]);
}
