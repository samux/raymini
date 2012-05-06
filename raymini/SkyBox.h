#pragma once

#include "Object.h"

#include "Material.h"

class Texture;
class Material;
class Mesh;

/** Skybox object */
class SkyBox: public Object {

public:
    virtual ~SkyBox();

    static constexpr const char *textureFileName = "textures/skybox.ppm";
    static constexpr const char *modelFileName = "models/skybox.off";

    static SkyBox *generateSkyBox(const SkyBoxMaterial *);

private:
    SkyBox(Texture *, const SkyBoxMaterial *, Mesh);
};
