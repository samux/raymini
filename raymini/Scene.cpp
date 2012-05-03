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

void printUsage(char * name) {
    cout << endl
         << "Choose a scene: " << name << " <scene>" << endl
         << "Possibilities are: " << endl
         << "\troom: simple room" << endl
         << "\trs: room with sphere" << endl
         << "\trsm: room with mirror sphere" << endl
         << "\trsglas: room with glass sphere" << endl
         << "\trsglos: room with glossy sphere" << endl
         << "\tlights: severals light and a ram" << endl
         << "\tmeshs: severals meshs and a light (moving ram)" << endl
         << "\toutdoor" << endl
         << "\tpool : pool table" << endl
         << "\tmesh <mesh_path>" << endl
         << endl;
    exit(1);
}

Scene::Scene(Controller *c, int argc, char **argv) :
    red      ({c, 1.f, .5f, {1.f, 0.f, 0.f}}),
    green    ({c, 1.f, .5f, {0.f, 1.f, 0.f}}),
    blue     ({c, 1.f, .5f, {.0f, 0.f, 1.f}}),
    white    ({c, 1.f, .5f, {1.f, 1.f, 1.f}}),
    black    ({c, 1.f, .5f, {0.f, 0.f, 0.f}}),
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
    else if(!id.compare("lights")) buildMultiLights();
    else if(!id.compare("meshs")) buildMultiMeshs();
    else if(!id.compare("outdoor")) buildOutdor();
    else if(!id.compare("pool")) buildPool();
    else if(!id.compare("mesh"))
        buildMesh(meshPath, new Material(c, 1.f, 1.f, Vec3Df (1.f, .6f, .2f)));
    else printUsage(argv[0]);

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
        if(glass) {
            sphere->setTrans({0,0,1.5});
            glass->setObject(sphere);
        }
        if(dynamic_cast<Mirror*>(sphereMat))sphere->setTrans({0,0,0});
        objects.push_back(sphere);
    }

    lights.push_back(new Light({0.f, 0.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, .7f));
}

void Scene::buildMesh(const std::string & path, Material *mat) {
    Mesh mesh;
    mesh.loadOFF(path);
    mesh.scale(1.f/Object::computeBoundingBox(mesh).getRadius());
    objects.push_back(new Object(mesh, mat));

    lights.push_back(new Light({1.f, 1.f, 1.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildMultiLights() {
    auto ramMat = new Material(controller, 1.f, 1.f, Vec3Df (1.f, .6f, .2f));

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    objects.push_back(new Object(groundMesh, &white, "Ground"));

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram"));

    lights.push_back(new Light({0.f, -3.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 0.f, 0.f}, 1.f));
    lights.push_back(new Light({-2.f, 2.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {0.f, 1.f, 0.f}, 1.f));
    lights.push_back(new Light({2.f, 2.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {0.f, 0.f, 1.f}, 1.f));
}

void Scene::buildMultiMeshs() {
    auto ramMat = new Material(controller, 1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    auto gargMat = new Material(controller, 0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    objects.push_back(new Object(groundMesh, &groundMat, "Ground"));

    Mesh wallMesh;
    wallMesh.loadOFF("models/wall.off");
    objects.push_back(new Object(wallMesh, &mirrorMat, "Left wall", {-1.9f, 0.f, 1.5f}));

    wallMesh.rotate({0.f, 0.f, 1.f}, 3*M_PI/2);
    objects.push_back(new Object(wallMesh, &red, "Back wall", {0.f, 1.9f, 1.5}));

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram", {-1.f, 0.f, 0.f}, {0,-.5,0}));

    Mesh gargMesh;
    gargMesh.loadOFF("models/gargoyle.off");
    objects.push_back(new Object(gargMesh, gargMat, "Gargoyle", {-1.f, 1.0f, 0.f}));

    Mesh rhinoMesh;
    rhinoMesh.loadOFF("models/rhino.off");
    objects.push_back(new Object(rhinoMesh, &rhinoMat, "Rhino", {1.f, 0.f, 0.4f}));

    lights.push_back(new Light({2.f, -3.f, 5.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildOutdor() {
    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.scale(5);
    objects.push_back(new Object(groundMesh, &groundMat, "Ground"));

    Mesh wallMesh;
    wallMesh.loadOFF("models/wall.off");
    objects.push_back(new Object(wallMesh, &mirrorMat, "Left wall", {-2.f, 0.f, 1.5f}));

    Mesh rhinoMesh;
    rhinoMesh.loadOFF("models/rhino.off");
    objects.push_back(new Object(rhinoMesh, &rhinoMat, "Rhino", {1.f, 0.f, 0.4f}));

    Mesh skyBoxMesh;
    skyBoxMesh.loadOFF("models/skybox.off");
    objects.push_back(new Object(skyBoxMesh, &skyBoxMaterial, "Skybox"));

    lights.push_back(new Light({9.f, 9.f, 9.f}, 5.f, {1.f, 1.f, 1.f},
                               {1.f, 1.f, .4f}, 1.f));
}

void Scene::buildPool() {
    auto pool = new Material(controller, 1.f, 0.f, {0.f, .3f, .1f},
                             [](const Vertex & v) -> float {
                                 return min(1.f, 0.4f+Perlin(0.5f, 4, 10)(v.getPos()));
                             });

    const vector<Vec3Df> color = {
        {1.f, 1.f, .0f},
        {1.f, .0f, .5f}, {.3f, .3f, .8f},
        {.0f, 1.f, .0f}, {.0f, .0f, .0f}, {1.f, .8f, .2f},
        {.0f, .6f, .4f}, {.2f, .2f, 1.f}, {1.f, .0f, .0f}, {.5f, .5f, .1f},
        {.4f, .7f, .2f}, {.5f, 1.f, .5f}, {.7f, .2f, .0f}, {.0f, .0f, .3f}, {.7f, .5f, .3f}
    };

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere.off");

    const float height = Object::computeBoundingBox(sphereMesh).getHeight()/2;
    const float delta = sqrt(3.f)*height;
    groundMesh.scale(height*15);

    objects.push_back(new Object(groundMesh, pool, "Pool"));

    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j <= i ; j++) {
            auto ball = new Material(controller, 1.f, 1.f, color[(i*(i+1))/2+j], .05f, 50);
            objects.push_back(new Object(sphereMesh, ball, "Ball", {-i*delta, (2*j-i)*height, height}));
        }

    lights.push_back(new Light({5.f, 5.f, 20.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, .7f));
}
