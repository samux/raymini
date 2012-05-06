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

Vec3Df Texture::getColor(unsigned int x, unsigned int y) const{
    unsigned int index = (x + y * width) * 3;

    return Vec3Df(values[index], values[index+1], values[index+2]);
}
