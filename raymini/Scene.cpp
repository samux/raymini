// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <iostream>
#include <string>

#include "Scene.h"

#include "Noise.h"
#include "SkyBoxMaterial.h"

using namespace std;

void printUsage() {
    cout << "Using default scene. "
         << "Other possibilities are: " << endl
         << "room: simple room" << endl
         << "rs: room with sphere" << endl
         << "rsm: room with mirror sphere" << endl
         << "rsglas: room with glass sphere" << endl
         << "rsglos: room with glossy sphere" << endl
         << "mesh <mesh_path>" << endl
         << endl;
}

Scene::Scene(Controller *c, int argc, char **argv) :
    red      ({c, 1.f, 1.f, {1.f, 0.f, 0.f}}),
    green    ({c, 1.f, 1.f, {0.f, 1.f, 0.f}}),
    blue     ({c, 1.f, 1.f, {.0f, 0.f, 1.f}}),
    white    ({c, 1.f, 1.f, {1.f, 1.f, 1.f}}),
    black    ({c, 1.f, 1.f, {0.f, 0.f, 0.f}}),
    glossyMat({c, 1.f, 1.f, {1.f, 0.f, 0.f}, .1f}),
    groundMat({c, 1.f, 0.f, {.2f, .6f, .2f},
                [](const Vertex & v) -> float {
                    return Perlin(0.5f, 4, 10)(v.getPos());
                }}),
    rhinoMat ({c, 1.f, 0.2f, {.6f, .6f, .7f},
                [](const Vertex & v) -> float {
                    return sqrt(fabs(sin(2 * M_PI * Perlin(0.5f, 4, 5)(v.getPos()))));
                }}),
    mirrorMat({c}),
    skyBoxMaterial({c, "textures/skybox.ppm"}),
    controller(c) {

    string id(argc>1?argv[1]:"");
    string meshPath(argc>2?argv[2]:"");

    if(!id.compare("room")) buildRoom();
    else if(!id.compare("rs")) buildRoom(&red);
    else if(!id.compare("rsm")) buildRoom(&mirrorMat);
    else if(!id.compare("rsglas")) buildRoom(new Glass(c, 1.1f));
    else if(!id.compare("rsglos")) buildRoom(&glossyMat);
    else if(!id.compare("mesh"))
        buildMesh(meshPath, new Material(c, 1.f, 1.f, Vec3Df (1.f, .6f, .2f)));
    else buildDefaultScene();

    updateBoundingBox ();
}

Scene::~Scene () {
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

void Scene::buildRoom(Material *sphereMat) {
    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");

    objects.push_back(new Object(groundMesh, &white, "Ground"));

    groundMesh.rotate({0,1,0}, M_PI);
    objects.push_back(new Object(groundMesh, &white, "Ceiling", {0, 0, 4}));

    groundMesh.rotate({0,1,0}, M_PI/2);
    objects.push_back(new Object(groundMesh, &red, "Right Wall", {2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &green, "Back Wall", {0, 2, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &blue, "Left Wall", {-2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &black, "Front Wall", {0, -2, 2}));

    if(sphereMat) {
        Mesh sphereMesh;
        sphereMesh.loadOFF("models/sphere.off");
        auto sphere = new Object(sphereMesh, sphereMat, "Sphere", {0, 0, 1});
        auto glass = dynamic_cast<Glass*>(sphereMat);
        if(glass) glass->setObject(sphere);
        objects.push_back(sphere);
    }

    lights.push_back(new Light({0.f, 0.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildMesh(const std::string & path, Material *mat) {
    Mesh mesh;
    mesh.loadOFF(path);
    mesh.scale(1.f/Object::computeBoundingBox(mesh).getRadius());
    objects.push_back(new Object(mesh, mat));

    lights.push_back(new Light({1.f, 1.f, 1.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildDefaultScene () {
    printUsage();

    auto ramMat = new Material(controller, 1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    auto gargMat = new Material(controller, 0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));

    //---------- GROUND---------//
    Mesh groundMesh;
    groundMesh.loadOFF ("models/ground.off");
    Object * ground = new Object(groundMesh, &groundMat, "Ground");
    objects.push_back (ground);

    //-------- CEILING---------//
    Mesh ceilingMesh;
    ceilingMesh.loadOFF ("models/window.off");
    ceilingMesh.rotate(Vec3Df(0,1,0), M_PI);

    Object * ceiling = new Object(ceilingMesh, &blue, "Ceiling");
    ceiling->setTrans(Vec3Df(0, 0, 3.0));
    objects.push_back (ceiling);

    //---------- WALLS----------//
    Mesh wallMesh;
    wallMesh.loadOFF ("models/wall.off");

    Object * leftWall = new Object(wallMesh, &mirrorMat, "Left wall");
    leftWall->setTrans(Vec3Df(-1.95251, 0, 1.5));
    objects.push_back (leftWall);

    Mesh backWallMesh(wallMesh);
    backWallMesh.rotate(Vec3Df(0, 0, 1), 3*M_PI/2);
    Object * backWall = new Object(backWallMesh, &red, "Back wall");
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
    Object * rhino = new Object(rhinoMesh, &rhinoMat, "Rhino");
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
    Object * skyBox = new Object(skyBoxMesh, &skyBoxMaterial, "Skybox");
    skyBox->setEnabled(false);
    objects.push_back(skyBox);


    Light * l = new Light(Vec3Df (0, 0, 3), 0.5, Vec3Df(0, 0, 1), Vec3Df (1.f, 1.f, 1.f), 1.0f);
    lights.push_back (l);
    Light * l1 = new Light(Vec3Df (.5f, 3.f, 2.5f), 0.5, Vec3Df(0, 0, 1), Vec3Df (1.0f, 0.0f, 0.0f), 1.0f);
    lights.push_back (l1);
}
