#pragma once

#include "Material.h"
#include "Texture.h"
#include "Vertex.h"
#include "Vec3D.h"
#include "Object.h"

enum CubeSide {
    TOP,
    BOTTOM,
    ONE,
    TWO,
    THREE,
    FOUR
};

/** Special Material for the skybox */
class SkyBoxMaterial : public Material {

public:
    SkyBoxMaterial(const char* textureFileName);
    virtual ~SkyBoxMaterial();

    virtual Vec3Df genColor(const Vec3Df & camPos, const Vertex & closestIntersection, Object *intersectedObject) const;

    Vec3Df getColor(CubeSide side, int u, int v) const;

protected:
    Texture texture;
};
