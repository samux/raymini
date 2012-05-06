#include "SkyBox.h"

#include <iostream>

#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Controller.h"

using namespace std;

SkyBox::SkyBox(Texture *t, const SkyBoxMaterial *mat, Mesh m):
    Object(m, mat, "Skybox")
{
    texture = t;

    // Box size in texture space
    unsigned int boxWidth = texture->getWidth()/4;
    unsigned int boxHeight = texture->getHeight()/3;

    vector<Triangle> &ts(mesh.getTriangles());

    // TWO
    ts[0].setUV(0, 2*boxWidth-0, 1*boxHeight-0+2);
    ts[0].setUV(1, 1*boxWidth-0, 1*boxHeight-0+2);
    ts[0].setUV(2, 2*boxWidth-0, 2*boxHeight-0-2);
    ts[1].setUV(0, 1*boxWidth-0, 1*boxHeight-0+2);
    ts[1].setUV(1, 1*boxWidth-0, 2*boxHeight-0-2);
    ts[1].setUV(2, 2*boxWidth-0, 2*boxHeight-0-2);

    // TOP
    ts[2].setUV(0, 1*boxWidth-0, 1*boxHeight-0);
    ts[2].setUV(1, 2*boxWidth-0, 1*boxHeight-0);
    ts[2].setUV(2, 2*boxWidth-0, 0*boxHeight-0);
    ts[3].setUV(0, 1*boxWidth-0, 0*boxHeight-0);
    ts[3].setUV(1, 1*boxWidth-0, 1*boxHeight-0);
    ts[3].setUV(2, 2*boxWidth-0, 0*boxHeight-0);

    // FOUR
    ts[4].setUV(0, 3*boxWidth-0, 2*boxHeight-0-2);
    ts[4].setUV(1, 4*boxWidth-1, 1*boxHeight-0+2);
    ts[4].setUV(2, 3*boxWidth-0, 1*boxHeight-0+2);
    ts[5].setUV(0, 4*boxWidth-1, 2*boxHeight-0-2);
    ts[5].setUV(1, 4*boxWidth-1, 1*boxHeight-0+2);
    ts[5].setUV(2, 3*boxWidth-0, 2*boxHeight-0-2);

    //BOTTOM
    ts[6].setUV(0, 2*boxWidth-0, 3*boxHeight-1);
    ts[6].setUV(1, 2*boxWidth-0, 2*boxHeight-0);
    ts[6].setUV(2, 1*boxWidth-0, 3*boxHeight-1);
    ts[7].setUV(0, 2*boxWidth-0, 2*boxHeight-0);
    ts[7].setUV(1, 1*boxWidth-0, 2*boxHeight-0);
    ts[7].setUV(2, 2*boxWidth-0, 3*boxHeight-1);

    // ONE
    ts[8].setUV(0, 1*boxWidth-0, 1*boxHeight-0+2);
    ts[8].setUV(1, 0*boxWidth-0, 1*boxHeight-0+2);
    ts[8].setUV(2, 1*boxWidth-0, 2*boxHeight-0-2);
    ts[9].setUV(0, 1*boxWidth-0, 2*boxHeight-0-2);
    ts[9].setUV(1, 0*boxWidth-0, 1*boxHeight-0+2);
    ts[9].setUV(2, 0*boxWidth-0, 2*boxHeight-0-2);

    // THREE
    ts[10].setUV(0, 3*boxWidth-0, 1*boxHeight-0+2);
    ts[10].setUV(1, 2*boxWidth-0, 2*boxHeight-0-2);
    ts[10].setUV(2, 3*boxWidth-0, 2*boxHeight-0-2);
    ts[11].setUV(0, 2*boxWidth-0, 1*boxHeight-0+2);
    ts[11].setUV(1, 2*boxWidth-0, 2*boxHeight-0-2);
    ts[11].setUV(2, 3*boxWidth-0, 1*boxHeight-0+2);
}

SkyBox::~SkyBox()
{
    delete texture;
    delete mat;
}

SkyBox *SkyBox::generateSkyBox(const SkyBoxMaterial *m) {
    Texture *t = new Texture();
    t->loadPPM(textureFileName);
    Mesh mesh;
    mesh.loadOFF(modelFileName);
    mesh.scale(10);
    SkyBox *skybox = new SkyBox(t, m, mesh);
    return skybox;
}
