#pragma once

#include "Vec3D.h"

/** Load PPM */
class Texture
{
	public:
        unsigned int width,
                     height,
                     max;
		unsigned char *v;

		Texture();
		~Texture();
		void loadPPM(const char *name);

        Vec3Df getColor(unsigned int x, unsigned int y) const;
};
