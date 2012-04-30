// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"

#include "Noise.h"
#include "SkyBoxMaterial.h"
#include "Controller.h"

using namespace std;

Scene::Scene(Controller *c):
    controller(c)
{
    buildDefaultScene ();
    updateBoundingBox ();
}

Scene::~Scene () {
}

void Scene::updateBoundingBox () {
    if (objects.empty ())
        bbox = BoundingBox ();
    else {
        bbox = objects[0].getBoundingBox ();
        for (unsigned int i = 1; i < objects.size (); i++)
            bbox.extendTo (objects[i].getBoundingBox ());
    }
}

void Scene::buildDefaultScene () {
    groundMat = new Material(controller, 1.f, 0.f, Vec3Df (.2f, 0.6f, .2f),
                                       [](const Vertex & v){
                                           static const Perlin perlin(0.5f, 4, 10);
                                           return perlin(v.getPos());
                                       });
    blue = new Material(controller, 1.f, 1.f, Vec3Df (.0f, 0.f, 1.f));
    red = new Material(controller, 1.f, 1.f, Vec3Df (1.0f, 0.f, 0.f));
    ramMat = new Material(controller, 1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    rhinoMat = new Material(controller, 1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f),
                                      [](const Vertex & v) -> float{
                                          static const Perlin perlin(0.5f, 4, 5);
                                          return sqrt(fabs(sin(2 * M_PI * perlin(v.getPos()))));
                                      });
    gargMat = new Material(controller, 0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));
    skyBoxMaterial = new SkyBoxMaterial(controller, "textures/skybox.ppm");
    mirrorMaterial = new Mirror(controller);

    Mesh groundMesh;
    groundMesh.loadOFF ("models/ground.off");
    Object ground (groundMesh, groundMat);
    objects.push_back (ground);
    objectsNames.push_back("Ground");

    Mesh wallMesh;
    wallMesh.loadOFF ("models/wall.off");

    Object leftWall(wallMesh, mirrorMaterial);
    leftWall.setTrans(Vec3Df(-1.95251, 0, 1.5));
    objects.push_back (leftWall);
    objectsNames.push_back("Left wall");

    Mesh backWallMesh(wallMesh);
    backWallMesh.rotate(Vec3Df(0, 0, 1), 3*M_PI/2);
    Object backWall(backWallMesh, red);
    backWall.setTrans(Vec3Df(0, 1.95251, 1.5));
    objects.push_back (backWall);
    objectsNames.push_back("Back wall");

    Mesh ramMesh;
    ramMesh.loadOFF ("models/ram.off");
    Object ram (ramMesh, ramMat);
    ram.setTrans (Vec3Df (-1.f, -1.0f, 0.f));
    objects.push_back (ram);
    objectsNames.push_back("Ram");

    Mesh rhinoMesh;
    rhinoMesh.loadOFF ("models/rhino.off");
    Object rhino (rhinoMesh, rhinoMat);
    rhino.setTrans (Vec3Df (1.f, 0.f, 0.4f));
    objects.push_back (rhino);
    objectsNames.push_back("Rhino");

    Mesh gargMesh;
    gargMesh.loadOFF ("models/gargoyle.off");
    Object garg (gargMesh, gargMat);
    garg.setTrans (Vec3Df (-1.f, 1.0f, 0.f));
    objects.push_back (garg);
    objectsNames.push_back("Gargoyle");

    Mesh skyBoxMesh;
    skyBoxMesh.loadOFF("models/skybox.off");
    Object skyBox(skyBoxMesh, skyBoxMaterial);
    skyBox.setEnabled(false);
    objects.push_back(skyBox);
    objectsNames.push_back("Skybox");

    Light l (Vec3Df (.5f, -3.f, 5.5f), 0.5, Vec3Df(0, 0, 1), Vec3Df (1.f, 1.f, 1.f), 1.0f);
    lights.push_back (l);
    Light l1 (Vec3Df (.5f, 3.f, 5.5f), 0.5, Vec3Df(0, 0, 1), Vec3Df (1.0f, 0.0f, 0.0f), 1.0f);
    lights.push_back (l1);
}
