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
    delete groundMat;
    delete blue;
    delete red;
    delete ramMat;
    delete rhinoMat;
    delete gargMat;
    delete skyBoxMaterial;
    delete mirrorMaterial;

    for(auto o : objects)
        delete o;

    for(auto l : lights)
        delete l;
}

void Scene::updateBoundingBox () {
    if (objects.empty ())
        bbox = BoundingBox ();
    else {
        for (const Object *o : objects) {
            if (o->isEnabled()) {
                bbox.extendTo(o->getBoundingBox().translate(o->getTrans()));
            }
        }
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

    //---------- GROUND---------//
    Mesh groundMesh;
    groundMesh.loadOFF ("models/ground.off");
    Object * ground = new Object(groundMesh, groundMat, "Ground");
    objects.push_back (ground);

    //-------- CEILING---------//
    Mesh ceilingMesh;
    ceilingMesh.loadOFF ("models/window.off");
    ceilingMesh.rotate(Vec3Df(0,1,0), M_PI);

    Object * ceiling = new Object(ceilingMesh, blue, "Ceiling");
    ceiling->setTrans(Vec3Df(0, 0, 3.0));
    objects.push_back (ceiling);

    //---------- WALLS----------//
    Mesh wallMesh;
    wallMesh.loadOFF ("models/wall.off");

    Object * leftWall = new Object(wallMesh, mirrorMaterial, "Left wall");
    leftWall->setTrans(Vec3Df(-1.95251, 0, 1.5));
    objects.push_back (leftWall);

    Mesh backWallMesh(wallMesh);
    backWallMesh.rotate(Vec3Df(0, 0, 1), 3*M_PI/2);
    Object * backWall = new Object(backWallMesh, red, "Back wall");
    backWall->setTrans(Vec3Df(0, 1.95251, 1.5));
    objects.push_back (backWall);

    //---------- RAM-----------//
    Mesh ramMesh;
    ramMesh.loadOFF ("models/ram.off");
    Object * ram = new Object(ramMesh, ramMat, "Ram");
    ram->setTrans (Vec3Df (-1.f, -1.0f, 0.f));
    objects.push_back (ram);

    //---------- RHINO----------//
    Mesh rhinoMesh;
    rhinoMesh.loadOFF ("models/rhino.off");
    Object * rhino = new Object(rhinoMesh, rhinoMat, "Rhino");
    rhino->setTrans (Vec3Df (1.f, 0.f, 0.4f));
    objects.push_back (rhino);

    //---------- GARGOYLE-------//
    Mesh gargMesh;
    gargMesh.loadOFF ("models/gargoyle.off");
    Object * garg = new Object(gargMesh, gargMat, "Gargoyle");
    garg->setTrans (Vec3Df (-1.f, 1.0f, 0.f));
    objects.push_back (garg);

    //---------- SKY BOX--------//
    Mesh skyBoxMesh;
    skyBoxMesh.loadOFF("models/skybox.off");
    Object * skyBox = new Object(skyBoxMesh, skyBoxMaterial, "Skybox");
    skyBox->setEnabled(false);
    objects.push_back(skyBox);


    Light * l = new Light(Vec3Df (0, 0, 3), 0.5, Vec3Df(0, 0, 1), Vec3Df (1.f, 1.f, 1.f), 1.0f);
    lights.push_back (l);
    Light * l1 = new Light(Vec3Df (.5f, 3.f, 2.5f), 0.5, Vec3Df(0, 0, 1), Vec3Df (1.0f, 0.0f, 0.0f), 1.0f);
    lights.push_back (l1);
}
