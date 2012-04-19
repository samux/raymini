// *********************************************************
// Model Class
// Author: Axel Schumacher (axelschumacherberry@gmail.com).
//
// Description:
// Provide current model state information, such as AA settings and others...
// *********************************************************

#include "Model.h"

static Model *instance = NULL;

Model *Model::getInstance()
{
	if (!instance) {
		instance = new Model();
	}

	return instance;
}

void setAntiAliasingType(AntiAliasingType type)
{
	antiAliasingType = type;
}

AntiAliasingType getAntiAliasingType() const
{
	return antiAliasingType;
}

void setAntiAliasingRaysPerPixel(unsigned int raysPerPixel)
{
	antiAliasingRaysPerPixel = raysPerPixel;
}

unsigned int getAntiAliasingRaysPerPixel() const
{
	return antiAliasingRaysPerPixel;
}
