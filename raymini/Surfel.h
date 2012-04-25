#pragma once

#include "Vec3D.h"
#include "Object.h"
#include "Material.h"

/**
 * A single surfel
 */
class Surfel {
private:
    Vec3Df position;
    Vec3Df normal;
    float radius;
    Vec3Df color;

    // Debug
    const Material *material;

public:
    Surfel(
            const Vec3Df &position,
            const Vec3Df &normal,
            const float &radius,
            const Vec3Df &color,
            const Material *material=NULL);
    ~Surfel();

    const Vec3Df &getPos() const;
    const Vec3Df &getNormal() const;
    const Vec3Df &getColor() const;
    float getRadius() const;
    const Material * getMaterial() const;

    /** Return an object representing the surfel */
    Object generateObject(unsigned int precision) const;
};
