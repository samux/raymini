#include "SkyBox.h"

#include <iostream>

#include "Material.h"
#include "Mesh.h"
#include "Controller.h"

using namespace std;

SkyBox::SkyBox(const SkyBoxMaterial *mat, Mesh m):
    Object(m, mat, "Skybox")
{
    // Box size in texture space
    float boxWidth = 0.25;
    float boxHeight = 1.0/3.0;

    vector<Triangle> &ts(mesh.getTriangles());

    // TWO
    ts[0].setUV(0, 2.0*boxWidth, 1.0*boxHeight+0.002);
    ts[0].setUV(1, 1.0*boxWidth, 1.0*boxHeight+0.002);
    ts[0].setUV(2, 2.0*boxWidth, 2.0*boxHeight-0.002);
    ts[1].setUV(0, 1.0*boxWidth, 1.0*boxHeight+0.002);
    ts[1].setUV(1, 1.0*boxWidth, 2.0*boxHeight-0.002);
    ts[1].setUV(2, 2.0*boxWidth, 2.0*boxHeight-0.002);

    // TOP
    ts[2].setUV(0, 1.0*boxWidth, 1.0*boxHeight);
    ts[2].setUV(1, 2.0*boxWidth, 1.0*boxHeight);
    ts[2].setUV(2, 2.0*boxWidth, 0.0*boxHeight);
    ts[3].setUV(0, 1.0*boxWidth, 0.0*boxHeight);
    ts[3].setUV(1, 1.0*boxWidth, 1.0*boxHeight);
    ts[3].setUV(2, 2.0*boxWidth, 0.0*boxHeight);

    // FOUR
    ts[4].setUV(0, 3.0*boxWidth, 2.0*boxHeight-0.002);
    ts[4].setUV(1, 4.0*boxWidth, 1.0*boxHeight+0.002);
    ts[4].setUV(2, 3.0*boxWidth, 1.0*boxHeight+0.002);
    ts[5].setUV(0, 4.0*boxWidth, 2.0*boxHeight-0.002);
    ts[5].setUV(1, 4.0*boxWidth, 1.0*boxHeight+0.002);
    ts[5].setUV(2, 3.0*boxWidth, 2.0*boxHeight-0.002);

    //BOTTOM
    ts[6].setUV(0, 2.0*boxWidth, 3.0*boxHeight);
    ts[6].setUV(1, 2.0*boxWidth, 2.0*boxHeight);
    ts[6].setUV(2, 1.0*boxWidth, 3.0*boxHeight);
    ts[7].setUV(0, 2.0*boxWidth, 2.0*boxHeight);
    ts[7].setUV(1, 1.0*boxWidth, 2.0*boxHeight);
    ts[7].setUV(2, 2.0*boxWidth, 3.0*boxHeight);

    // ONE
    ts[8].setUV(0, 1.0*boxWidth, 1.0*boxHeight+0.002);
    ts[8].setUV(1, 0.0*boxWidth, 1.0*boxHeight+0.002);
    ts[8].setUV(2, 1.0*boxWidth, 2.0*boxHeight-0.002);
    ts[9].setUV(0, 1.0*boxWidth, 2.0*boxHeight-0.002);
    ts[9].setUV(1, 0.0*boxWidth, 1.0*boxHeight+0.002);
    ts[9].setUV(2, 0.0*boxWidth, 2.0*boxHeight-0.002);

    // THREE
    ts[10].setUV(0, 3.0*boxWidth, 1.0*boxHeight+0.002);
    ts[10].setUV(1, 2.0*boxWidth, 2.0*boxHeight-0.002);
    ts[10].setUV(2, 3.0*boxWidth, 2.0*boxHeight-0.002);
    ts[11].setUV(0, 2.0*boxWidth, 1.0*boxHeight+0.002);
    ts[11].setUV(1, 2.0*boxWidth, 2.0*boxHeight-0.002);
    ts[11].setUV(2, 3.0*boxWidth, 1.0*boxHeight+0.002);
}

SkyBox::~SkyBox() {}

SkyBox *SkyBox::generateSkyBox(const SkyBoxMaterial *m) {
    Mesh mesh;
    mesh.loadOFF(modelFileName);
    mesh.scale(10);
    SkyBox *skybox = new SkyBox(m, mesh);
    return skybox;
}
