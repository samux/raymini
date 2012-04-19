// *********************************************************
// Model Class
// Author: Axel Schumacher (axelschumacherberry@gmail.com).
// *********************************************************

#include "Model.h"

static Model *instance = NULL;

Model::Model():
	antiAliasingType(NO_ANTIALIASING),
	antiAliasingRaysPerPixel(1)
{}

Model *Model::getInstance()
{
	if (!instance) {
		instance = new Model();
	}

	return instance;
}

void Model::setAntiAliasingType(AntiAliasingType type)
{
	antiAliasingType = type;
}

AntiAliasingType Model::getAntiAliasingType() const
{
	return antiAliasingType;
}

void Model::setAntiAliasingRaysPerPixel(unsigned int raysPerPixel)
{
	antiAliasingRaysPerPixel = raysPerPixel;
}

unsigned int Model::getAntiAliasingRaysPerPixel() const
{
	return antiAliasingRaysPerPixel;
}
