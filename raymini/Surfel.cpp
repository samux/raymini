#include "Surfel.h"
#include "Vertex.h"
#include "Triangle.h"

#include <vector>

using namespace std;

Surfel::Surfel(
        const Vec3Df &position,
        const Vec3Df &normal,
        const float &radius,
        const Vec3Df &color,
        const Material *material):
    position(position),
    normal(normal),
    radius(radius),
    color(color),
    material(material)
{}

Surfel::~Surfel() {}

Object *Surfel::generateObject(unsigned int precision) const{
    vector<Vertex> vertices;
    vector<Triangle> triangles;

    // Put the origin
    vertices.push_back(Vertex(position, normal));

    // Have basis
    Vec3Df upVector = normal.getOrthogonal();
    Vec3Df rightVector = Vec3Df::crossProduct(upVector, normal);

    //Handle critical case
    if (precision < 3) {
        precision = 3;
    }

    float angle = 0;
    float deltaAngle = 2.0*M_PI/(float)precision;

    for (unsigned int i=1; i<=precision; i++) {
        Vec3Df offset = cos(angle)*rightVector + sin(angle)*upVector;
        offset.normalize();
        offset *= 0.05;
        Vec3Df newPosition = position + offset;
        vertices.push_back(Vertex(newPosition, normal));
        angle += deltaAngle;
        unsigned int previousIndex = i-1==0 ? precision : i-1;
        triangles.push_back(Triangle(previousIndex, i, 0));
    }

    return new Object(Mesh(vertices, triangles), material);
}

const Vec3Df &Surfel::getPos() const {
    return position;
}

const Vec3Df &Surfel::getNormal() const {
    return normal;
}

const Vec3Df &Surfel::getColor() const {
    return color;
}

float Surfel::getRadius() const {
    return radius;
}

const Material * Surfel::getMaterial() const {
    return material;
}
