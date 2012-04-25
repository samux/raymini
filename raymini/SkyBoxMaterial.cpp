#include "SkyBoxMaterial.h"

#include <iostream>

using namespace std;

// Mesh dependent
#define SKY_BOX_DISTANCE 10

// Texture dependent
#define BOX_SIZE 512

void getSide(Vec3Df intersection, CubeSide &side, int &u, int &v) {
    bool isTop = intersection[2] == SKY_BOX_DISTANCE,
        isBottom = intersection[2] == -SKY_BOX_DISTANCE,
        isOne = intersection[0] == SKY_BOX_DISTANCE,
        isTwo = intersection[1] == -SKY_BOX_DISTANCE,
        isThree = intersection[0] == -SKY_BOX_DISTANCE,
        isFour = intersection[1] == SKY_BOX_DISTANCE;

    float fu, fv;

    if (isTop || isBottom) {
        fu = -intersection[0];
        fv = -intersection[1];
        side = isTop ? TOP : BOTTOM;
    }
    else if (isOne || isThree) {
        fu = isThree ? intersection[1] : -intersection[1];
        fv = -intersection[2];
        side = isOne ? ONE : THREE;
    }
    else if (isTwo || isFour) {
        fu = isFour? intersection[0] : -intersection[0];
        fv = -intersection[2];
        side = isTwo ? TWO : FOUR;
    }
    else {
        cout << "ERROR: MUST HANDLE LACK OF PRECISION !\nFound "<<intersection<<endl;
        return;
    }

    u = (fu + SKY_BOX_DISTANCE) * BOX_SIZE / (2 * SKY_BOX_DISTANCE);
    v = (fv + SKY_BOX_DISTANCE) * BOX_SIZE / (2 * SKY_BOX_DISTANCE);

    //u = u < 0? 0 : u;
    //u = u >= BOX_SIZE ? BOX_SIZE-1 : u;
    if (!isTop && !isBottom) {
        v = v < 3? 3 : v;
        v = v >= BOX_SIZE-3 ? BOX_SIZE-4 : v;
    }
}

Vec3Df SkyBoxMaterial::getColor(CubeSide side, int u, int v) const{
    int offsetU = 0,
        offsetV = 0;

    switch (side) {
        case TOP:
            offsetU = BOX_SIZE;
            offsetV = 0;
            break;
        case BOTTOM:
            offsetU = BOX_SIZE;
            offsetV = 2 * BOX_SIZE;
            break;
        case ONE:
            offsetU = 0;
            offsetV = BOX_SIZE;
            break;
        case TWO:
            offsetU = BOX_SIZE;
            offsetV = BOX_SIZE;
            break;
        case THREE:
            offsetU = 2*BOX_SIZE;
            offsetV = BOX_SIZE;
            break;
        case FOUR:
            offsetU = 3*BOX_SIZE;
            offsetV = BOX_SIZE;
            break;
    }

    return texture.getColor(u+offsetU, v+offsetV);
}

SkyBoxMaterial::SkyBoxMaterial(const char* textureFileName):
    Material(1.0f, 0.0f, Vec3Df(0.0f, 0.0f, 1.0f)) // Blue for OpenGL
{
    texture.loadPPM(textureFileName);
}

SkyBoxMaterial::~SkyBoxMaterial()
{}

Vec3Df SkyBoxMaterial::genColor(const Vec3Df &, const Vertex & closestIntersection,
                                std::vector<Light>, Brdf::Type) const {
    CubeSide side;
    int u, v;
    getSide(closestIntersection.getPos(), side, u, v);
    return getColor(side, u, v);
}
