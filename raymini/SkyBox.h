#pragma once

#include "Object.h"

class SkyBoxMaterial;
class Mesh;

/** Skybox object */
class SkyBox: public Object {

public:
    virtual ~SkyBox();

    static constexpr const char *textureFileName = "textures/skybox.ppm";
    static constexpr const char *modelFileName = "models/skybox.off";

    static SkyBox *generateSkyBox(const SkyBoxMaterial *);

private:
    SkyBox(const SkyBoxMaterial *, Mesh);
};
