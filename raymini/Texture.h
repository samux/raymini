#pragma once

#include "Vec3D.h"
#include "Ray.h"
#include "Vertex.h"
#include "NamedClass.h"
#include "NoiseUser.h"

#include <QImage>

/**
 * Textures are abstract classes dedicated to the operation of returning a value
 * out of an intersecting Ray. It has a method called getValue(Ray *)
 *
 * A MappedTexture is an abstract Texture returning a value out of two positions in [0,1]
 *
 * An ImageTexture is a MappedTexture returning a Vec3Df out of an image and a Ray
 *
 *
 * A ColorTexture is an abstract NamedClass having a representative color and
 * having a method called GetColor returning a Vec3Df out of a Ray
 * A MappedColorTexture is an abstract ColorTexture and MappedTexture implementing getColor(Ray *)
 * A ImageColorTexture is a MappedColorTexture implementing getColor(float, float)
 * A SingleColorTexture is a ColorTexture and a Texture returning nothing but its color
 * A NoiseColorTexture is a SingleColorTexture plus a noise function
 * A DebugColorTexture is a MappedColorTexture returning
 * a basic horrible checkerboard
 * A ImageColorTexture is a ColorTexture and an ImageTexture returning a color out of an image
 *
 * A NormalTexture is a NamedClass having a method called getNormal returning a Vec3Df out of a Ray
 * A MeshNormalTexture is a NormalTexture and a Texture returning the normal at the point
 * intersected by the Ray
 * An ImageNormalTexture is an ImageTexture and a NormalTexture returning the normal depending
 * of the point intersected by the Ray and the image
 * A NoiseNormalTexture is a NormalTexture and a MappedTexture returning a normal depending
 * of the position
 *
 * Here is the graph:
 *             
 *  ColorTexture-----------                      Texture         NormalTexture---
 *       |            |    \                        |                |     |  \
 *  SingleColorTexture|    |            ------MappedTexture----      |     |MeshNormalTexture
 *       |            |    |           /            |          \     |     |
 *  NoiseColorTexture |MappedColorTexture   ImageTexture NoiseNormalTexture|
 *                    |    |                     /      \                  |
 *                    |DebugColorTexture        /        -----------       |
 *                    |        -----------------                    \      |
 *                    |       /                                ImageNormalTexture
 *              ImageColorTexture
 */

/////////////
// GENERIC //
/////////////

/** Generic texture [ABSTRACT] */
template <typename T>
class Texture {
public:
    /**
     * To be implemented
     * Return the texture value of a point intersected by the ray.
     * Return default value to be specified if the ray didn't intersect
     **/
    virtual T getValue(Ray *) const = 0;
};

/** Mapped generic texture [ABSTRACT] */
template <typename T>
class MappedTexture: public Texture<T> {
public:
    /**
     * @override
     * Will now compute u,v coordinates and call getColor(u, v, uScale, vScale)
     */
    virtual T getValue(Ray *intersectingRay) const;

protected:
    /**
     * To be implemented
     * Return mapped value
     * Be sure to configure u,v for each vertex of the mesh
     * Assume that u and v are in [0,1]
     */
    virtual T getValue(float u, float v) const = 0;

private:
    /**
     * Will modify u and v according to the scales got from the intersected object mesh
     */
    static void adaptUV(float &u, float &v, float uScale, float vScale);
};

/** Load image from file */
class ImageTexture: public MappedTexture<Vec3Df> {
public:
    ImageTexture(const char *fileName);
    ~ImageTexture();

    virtual Vec3Df getValue(Ray *intersectingRay) const;

    const QImage *getImage() const {return image;}
    const char *getImageFileName() const {return imageFileName.c_str();}
    /** Return true if loading successful */
    bool loadImage(const char *name);

protected:
    std::string imageFileName;
    QImage *image;

    /** @override */
    virtual Vec3Df getValue(float u, float v) const;
};

///////////////////
////// COLOR //////
///////////////////

/** Class to be inherited by any texture returning colors [ABSTRACT] */

class ColorTexture: public NamedClass
{
public:
    ColorTexture(Vec3Df color=Vec3Df(), std::string name="Color Texture");
    virtual ~ColorTexture();

    /**
     * Will be called mostly by Material.genColor
     */
    virtual Vec3Df getColor(Ray *intersectingRay) const = 0;

    Vec3Df getRepresentativeColor() const;
    void setRepresentativeColor(Vec3Df c);

    enum Type {
        SingleColor,
        Debug,
        Noise,
        Image
    };

    Type getType() const;

protected:
    /** Representative color for OpenGL */ 
    Vec3Df color;
};

/** A mapped texture for ColorTexture [ABSTRACT] */
class MappedColorTexture: public ColorTexture, public MappedTexture<Vec3Df> {
public:
    MappedColorTexture(Vec3Df color=Vec3Df(), std::string name="Mapped Color Texture");
    virtual ~MappedColorTexture();

    /** @override */
    virtual Vec3Df getColor(Ray *intersectingRay) const;

protected:
    /**
     * @override
     * Still to be implemented
     */
    virtual Vec3Df getValue(float u, float v) const = 0;
};

/**
 * Basic debug texture, no memory space needed
 * Two colors checkerboard
 */
class DebugColorTexture: public MappedColorTexture {
public:
    DebugColorTexture(std::string name="Debug Color Texture");
    virtual ~DebugColorTexture();

    /** @override */
    virtual Vec3Df getValue(float u, float v) const;

protected:
    /** @override */
    virtual Vec3Df getColor(float u, float v) const;
};

/** Unique color texture */
class SingleColorTexture: public ColorTexture {
public:
    /** Default name is "Single Color Texture "+color.toString() */
    SingleColorTexture(Vec3Df color, std::string name=std::string());
    virtual ~SingleColorTexture();

    /**
     * @override
     * Will return the color
     */
    virtual Vec3Df getColor(Ray *) const;
};

/** Noise-based color texture */
class NoiseColorTexture: public SingleColorTexture, public NoiseUser {
public:
    NoiseColorTexture(Vec3Df color,
                      NoiseUser::Predefined p,
                      std::string name="Noise Texture");
    virtual ~NoiseColorTexture();

    /**
     * @override
     * Will return the noise-based color depending
     * of the intersected pixel position
     */
    virtual Vec3Df getColor(Ray *) const;
};

/** Image color texture */
class ImageColorTexture: public ColorTexture, public ImageTexture {
public:
    ImageColorTexture(const char *fileName, std::string name="Image Color Texture");
    ~ImageColorTexture();

    /** @override */
    virtual Vec3Df getColor(Ray *) const;
};

////////////////////
////// NORMAL //////
////////////////////

/**
 * Class to be inherited by any normal returning texture
 */
class NormalTexture: public NamedClass {
public:
    NormalTexture(std::string name="Normal Texture");
    virtual ~NormalTexture();

    /**
     * Will be called whenever a normal is wanted from this texture
     */
    virtual Vec3Df getNormal(Ray *) const = 0;

    enum Type {
        Mesh,
        Noise,
        Image
    };

    Type getType() const;
};

/**
 * Basic normal texture returning pondered mesh normal
 */
class MeshNormalTexture: public NormalTexture {
public:
    MeshNormalTexture(std::string name="Mesh Normal Texture");
    virtual ~MeshNormalTexture();

    /**
     * @override
     * Return default mesh normal at this point
     */
    virtual Vec3Df getNormal(Ray *) const;
};

/**
 * Normal texture based on an image
 */
class ImageNormalTexture: public NormalTexture, public ImageTexture {
public:
    ImageNormalTexture(const char *fileName, std::string name="Image Normal Texture");
    virtual ~ImageNormalTexture();

    /**
     * @override
     * Return image color transformed to a normal according to intersection point
     */
    virtual Vec3Df getNormal(Ray *) const;
};

/**
 * Normal texture based on noise
 */
class NoiseNormalTexture: public NormalTexture, public NoiseUser {
public:
    NoiseNormalTexture(NoiseUser::Predefined p,
                       Vec3Df offset,
                       std::string name="Noise Normal Texture");
    virtual ~NoiseNormalTexture();

    /**
     * @override
     * Will return the noise-based normal depending
     * of the intersected pixel position
     */
    virtual Vec3Df getNormal(Ray *) const;

    void setOffset(Vec3Df o) {offset=o;}
    Vec3Df getOffset() const {return offset;}

protected:
    Vec3Df offset;
};
