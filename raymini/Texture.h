#pragma once

#include "Vec3D.h"
#include "Ray.h"
#include "Vertex.h"

#include <QImage>

/** Generic texture [ABSTRACT] */
class Texture
{
public:
    Texture(std::string name="Texture");
    virtual ~Texture();

    /**
     * To be implemented
     * Return the texture color of a point intersected by the ray.
     * Return debug color if the ray didn't intersect
     **/
    virtual Vec3Df getColor(Ray *intersectingRay) const = 0;

    Vec3Df getRepresentativeColor() const;
    void setRepresentativeColor(Vec3Df c);

    std::string getName() const;

protected:
    /** Representative color for OpenGL */ 
    Vec3Df color;

    std::string name;
};

/** Mapped generic texture [ABSTRACT] */
class MappedTexture: public Texture {
public:
    MappedTexture(std::string name="Mapped Texture");
    virtual ~MappedTexture();

    /**
     * @override
     * Will now compute u,v coordinates and call getColor(u, v)
     */
    virtual Vec3Df getColor(Ray *intersectingRay) const;

    /**
     * To be implemented
     * Return mapped color
     * Be sure to configure u,v for each vertex of the mesh
     */
    virtual Vec3Df getColor(float u, float v) const = 0;
};

/** Load image from file */
class ImageTexture: public MappedTexture {
public:
    ImageTexture(const char *fileName, std::string name="Image Texture");
    ~ImageTexture();

    /** @override */
    virtual Vec3Df getColor(float u, float v) const;

protected:
    QImage *image;
};

/** Basic debug texture, no memory space needed */
class BasicTexture: public MappedTexture {
public:
    BasicTexture(std::string name="Basic Texture");
    virtual ~BasicTexture();

    /** @override */
    virtual Vec3Df getColor(float u, float v) const;
};

/** Unique color texture */
class ColorTexture: public Texture {
public:
    ColorTexture(Vec3Df color, std::string name=std::string());
    virtual ~ColorTexture();

    /**
     * @override
     * Will return the color
     */
    virtual Vec3Df getColor(Ray *) const;
};

/** Noise-based texture */
class NoiseTexture: public ColorTexture {
public:
    NoiseTexture(Vec3Df color, float (*noise)(const Vertex &), std::string name="Noise Texture");
    virtual ~NoiseTexture();

    /**
     * @override
     * Will return the noise-based color depending
     * of the intersected pixel position
     */
    virtual Vec3Df getColor(Ray *) const;

protected:
    float (*noise)(const Vertex &);
};
