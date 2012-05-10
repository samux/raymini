#include "Texture.h"

#include <iostream>
#include <fstream>
#include <QColor>

#include "Mesh.h"
#include "Object.h"

using namespace std;

/********** MAPPED TEXTURE **********/

template <typename T>
T MappedTexture<T>::getValue(Ray *intersectingRay) const {
    if (!intersectingRay->intersect()) {
        cerr<<__FUNCTION__<<": cannot get the texture color!"<<endl;
        return T();
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

    const Mesh &mesh = intersectingRay->getIntersectedObject()->getMesh();

    adaptUV(interU, interV, mesh.getUScale(), mesh.getVScale());

    // Call abstract method
    return getValue(interU, interV);
}

template <typename T>
void MappedTexture<T>::adaptUV(float &u, float &v, float uScale, float vScale) {
    u -= (int)(u*uScale)/uScale;
    u *= uScale;

    v -= (int)(v*vScale)/vScale;
    v *= vScale;
}

/********** IMAGE TEXTURE ***********/

ImageTexture::ImageTexture(const char *fileName):
    imageFileName("No image loaded"),
    image(nullptr)
{
    loadImage(fileName);
}

bool ImageTexture::loadImage(const char *fileName) {
    auto newImage = new QImage(fileName);
    if (!newImage) {
        cerr<<__FUNCTION__<<": cannot read image "<<fileName<<endl;
        return false;
    }
    if (image) {
        delete image;
    }
    image = newImage;
    imageFileName = fileName;
    return true;
}

ImageTexture::~ImageTexture()
{
    if (image) {
        delete image;
    }
}

Vec3Df ImageTexture::getValue(Ray *intersectingRay) const {
    return MappedTexture<Vec3Df>::getValue(intersectingRay);
}

Vec3Df ImageTexture::getValue(float x, float y) const{
    if (!image) {
        return Vec3Df();
    }

    unsigned int width = image->width();
    unsigned int height = image->height();

    unsigned int u = x * width;
    if (u == width) {
        u = width - 1;
    }

    unsigned int v = y * height;
    if (v == height) {
        v = height - 1;
    }

    QColor pixel = QColor(image->pixel(u, v));

    return Vec3Df(pixel.red(), pixel.green(), pixel.blue())/255.0;
}

/******** COLOR TEXTURE *********/

ColorTexture::ColorTexture(Vec3Df color, string name):
    NamedClass(name),
    color(color)
{}

ColorTexture::~ColorTexture() {}

Vec3Df ColorTexture::getRepresentativeColor() const {
    return color;
}

void ColorTexture::setRepresentativeColor(Vec3Df c) {
    color = c;
}

ColorTexture::Type ColorTexture::getType() const {
    // Be careful with inheritance!
    if (dynamic_cast<const NoiseColorTexture*>(this)) {
        return Noise;
    }
    if (dynamic_cast<const SingleColorTexture*>(this)) {
        return SingleColor;
    }
    if (dynamic_cast<const DebugColorTexture*>(this)) {
        return Debug;
    }
    if (dynamic_cast<const ImageColorTexture*>(this)) {
        return Image;
    }
    cerr<<__FUNCTION__<<": unknown type!\n";
    return SingleColor;
}

/******* COLOR MAPPED TEXTURE ******/


MappedColorTexture::MappedColorTexture(Vec3Df color, string name):
    ColorTexture(color, name)
{}

MappedColorTexture::~MappedColorTexture() {}

Vec3Df MappedColorTexture::getColor(Ray *intersectingRay) const {
    return MappedTexture<Vec3Df>::getValue(intersectingRay);
}

/********* IMAGE COLOR TEXTURE *****/

ImageColorTexture::ImageColorTexture(const char *fileName, string name):
    ColorTexture(Vec3Df(1, 0, 1), name),
    ImageTexture(fileName)
{}

ImageColorTexture::~ImageColorTexture() {}

Vec3Df ImageColorTexture::getColor(Ray *ray) const {
    return ImageTexture::getValue(ray);
}

/*********** DEBUG COLOR TEXTURE ***********/

DebugColorTexture::DebugColorTexture(string name):
    MappedColorTexture(Vec3Df(1, 0, 1), name)
{}

DebugColorTexture::~DebugColorTexture() {}

Vec3Df DebugColorTexture::getValue(float x, float y) const {

    bool isXEven = x*2.0<1.0;
    bool isYEven = y*2.0<1.0;

    if (isXEven==isYEven) {
        return color;
    }
    return Vec3Df(1.0-color[0], 1.0-color[1], 1.0-color[2]);
}

Vec3Df DebugColorTexture::getColor(float u, float v) const {
    return getValue(u, v);
}

/************ SINGLE COLOR TEXTURE ***********/

SingleColorTexture::SingleColorTexture(Vec3Df color, string name):
    ColorTexture(color, name)
{
    if (name.empty()) {
        name = "Single Color Texture "+color.toString();
    }
}

SingleColorTexture::~SingleColorTexture()
{}

Vec3Df SingleColorTexture::getColor(Ray *) const {
    return color;
}

/******** NOISE COLOR TEXTURE **************/

NoiseColorTexture::NoiseColorTexture(Vec3Df color, NoiseUser::Predefined p, string name):
    SingleColorTexture(color, name),
    NoiseUser(p)
{}

NoiseColorTexture::~NoiseColorTexture() {}

Vec3Df NoiseColorTexture::getColor(Ray *ray) const {
    const Vertex &v = ray->getIntersection();
    return color*noise(v);
}

/************ NORMAL TEXTURE ***********/

NormalTexture::NormalTexture(string name):
    NamedClass(name)
{}

NormalTexture::~NormalTexture() {}

NormalTexture::Type NormalTexture::getType() const {
    if (dynamic_cast<const NoiseNormalTexture*>(this)) {
        return Noise;
    }
    if (dynamic_cast<const MeshNormalTexture*>(this)) {
        return Mesh;
    }
    if (dynamic_cast<const ImageNormalTexture*>(this)) {
        return Image;
    }
    cerr<<__FUNCTION__<<": unknown type!\n";
    return Mesh;
}


/******** MESH NORMAL TEXTURE **********/

MeshNormalTexture::MeshNormalTexture(std::string name):
    NormalTexture(name)
{}

MeshNormalTexture::~MeshNormalTexture() {}

Vec3Df MeshNormalTexture::getNormal(Ray *ray) const {
    return ray->getIntersection().getNormal();
}

/********* IMAGE NORMAL TEXTURE **********/

ImageNormalTexture::ImageNormalTexture(const char *fileName, string name):
    NormalTexture(name),
    ImageTexture(fileName)
{}

ImageNormalTexture::~ImageNormalTexture() {}

Vec3Df ImageNormalTexture::getNormal(Ray *ray) const {
    Vec3Df pointNormal = ray->getIntersection().getNormal();
    Vec3Df color = ImageTexture::getValue(ray);
    pointNormal += 2.0*color - Vec3Df(1, 1, 1);
    pointNormal.normalize();
    return pointNormal;
}

/******* NOISE NORMAL TEXTURE ************/

NoiseNormalTexture::NoiseNormalTexture(
        NoiseUser::Predefined p,
        Vec3Df offset,
        std::string name):
    NormalTexture(name),
    NoiseUser(p),
    offset(offset)
{}

NoiseNormalTexture::~NoiseNormalTexture() {}

Vec3Df NoiseNormalTexture::getNormal(Ray *r) const {
    const Vertex &v = r->getIntersection();
    Vec3Df normal = v.getNormal();
    Vec3Df noiseContribution = offset*noise(v);
    normal += noiseContribution*2.0 - Vec3Df(1, 1, 1);
    normal.normalize();
    return normal;
}
