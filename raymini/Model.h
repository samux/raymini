// *********************************************************
// Model Class
// Author: Axel Schumacher (axelschumacherberry@gmail.com).
//
// Description:
// Provide current model state information, such as AA settings and others...
//
// May be useful for future Observable/Observer design pattern...
// *********************************************************

#pragma once

#include <iostream>

enum AntiAliasingType {
    NO_ANTIALIASING,
    UNIFORM,
    POLYGONAL,
    STOCHASTIC
};

class Model
{
public:
    /** Default constructor */
    Model();

    /** Return the model instance of the program */
    static Model *getInstance();

    /** Set the antiAliasingType */
    void setAntiAliasingType(AntiAliasingType type);

    /** Get the antiAliasingType */
    AntiAliasingType getAntiAliasingType() const;

    /** Set the antiAliasingRaysPerPixel */
    void setAntiAliasingRaysPerPixel(unsigned int raysPerPixel);

    /** Get the antiAliasingRaysPerPixel */
    unsigned int getAntiAliasingRaysPerPixel() const;

    /** Set the ambientOcclusionRaysCount */
    void setAmbientOcclusionRaysCount(unsigned int count);

    /** Get the ambientOcclusionRaysCount */
    unsigned int getAmbientOcclusionRaysCount() const;

    /** Set the ambientOcclusionRadius */
    void setAmbientOcclusionRadius(float radius);

    /** Get the ambientOcclusionRadius */
    float getAmbientOcclusionRadius() const;

private:
    /** AntiAliasing to be used*/
    AntiAliasingType antiAliasingType;

    /** Number of rays used for each pixel */
    unsigned int antiAliasingRaysPerPixel;

    /** Ambient occlusion number of rays */
    unsigned int ambientOcclusionRaysCount;

    /** Ambient occlusion radius */
    float ambientOcclusionRadius;
};
