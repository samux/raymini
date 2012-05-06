// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <iostream>
#include <string>
#include <sstream>

#include "Scene.h"

#include "Noise.h"
#include "SkyBox.h"

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
         << "\tmg : mirror and glass" << endl
         << "\tmesh <mesh_path>" << endl
         << endl;
    exit(1);
}

Scene::Scene(Controller *c, int argc, char **argv) :
    red      ({c, "Red", 1.f, .5f, {1.f, 0.f, 0.f}}),
    green    ({c, "Green", 1.f, .5f, {0.f, 1.f, 0.f}}),
    blue     ({c, "Blue", 1.f, .5f, {.0f, 0.f, 1.f}}),
    white    ({c, "White", 1.f, .5f, {1.f, 1.f, 1.f}}),
    black    ({c, "Black", 1.f, .5f, {0.f, 0.f, 0.f}}),
    glossyMat({c, "Glossy", 1.f, 1.f, {1.f, 0.f, 0.f}, .1f}),
    groundMat({c, "Ground", 1.f, 0.f, {.2f, .6f, .2f},
                [](const Vertex & v) -> float {
                    return Perlin(0.5f, 4, 10)(v.getPos());
                }}),
    rhinoMat ({c, "Rhino", 1.f, 0.2f, {.6f, .6f, .7f},
                [](const Vertex & v) -> float {
                    return sqrt(fabs(sin(2 * M_PI * Perlin(0.5f, 4, 5)(v.getPos()))));
                }}),
    mirrorMat({c, "Mirror"}),
    controller(c) {

    string id(argc>1?argv[1]:"");
    string meshPath(argc>2?argv[2]:"");

    if(!id.compare("room")) buildRoom();
    else if(!id.compare("rs")) buildRoom(&red);
    else if(!id.compare("rsm")) buildRoom(&mirrorMat);
    else if(!id.compare("rsglas")) buildRoom(new Glass(c, "Glass", 1.1f));
    else if(!id.compare("rsglos")) buildRoom(&glossyMat);
    else if(!id.compare("lights")) buildMultiLights();
    else if(!id.compare("meshs")) buildMultiMeshs();
    else if(!id.compare("outdoor")) buildOutdor();
    else if(!id.compare("pool")) buildPool();
    else if(!id.compare("mg")) buildMirrorGlass();
    else if(!id.compare("mesh"))
        buildMesh(meshPath, new Material(c, "Mesh", 1.f, 1.f, Vec3Df (1.f, .6f, .2f)));
    else printUsage(argv[0]);

    updateBoundingBox ();
    }

Scene::~Scene () {
    for(auto o : objects)
        delete o;

    for(auto l : lights)
        delete l;

    for(auto m : materials)
        delete m;
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
    groundMesh.setSquareTextureMapping();

    objects.push_back(new Object(groundMesh, &white, "Ground"));

    groundMesh.rotate({0,1,0}, M_PI);
    objects.push_back(new Object(groundMesh, &white, "Ceiling", {0, 0, 4}));
    materials.push_back(&white);

    groundMesh.rotate({0,1,0}, M_PI/2);
    objects.push_back(new Object(groundMesh, &red, "Right Wall", {2, 0, 2}));
    materials.push_back(&red);

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &green, "Back Wall", {0, 2, 2}));
    materials.push_back(&green);

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &blue, "Left Wall", {-2, 0, 2}));
    materials.push_back(&blue);

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &black, "Front Wall", {0, -2, 2}));
    materials.push_back(&black);

    if(sphereMat) {
        materials.push_back(sphereMat);
        Mesh sphereMesh;
        sphereMesh.loadOFF("models/sphere.off");
        auto sphere = new Object(sphereMesh, sphereMat, "Sphere", {0, 0, 1});
        auto glass = dynamic_cast<Glass*>(sphereMat);
        if(glass) {
            sphere->setTrans({0,0,1.5});
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
    materials.push_back(mat);
}

void Scene::buildMultiLights() {
    auto ramMat = new Material(controller, "Ram", 1.f, 1.f, Vec3Df (1.f, .6f, .2f));

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    objects.push_back(new Object(groundMesh, &white, "Ground"));
    materials.push_back(&white);

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram"));
    materials.push_back(ramMat);

    lights.push_back(new Light({0.f, -3.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 0.f, 0.f}, 1.f));
    lights.push_back(new Light({-2.f, 2.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {0.f, 1.f, 0.f}, 1.f));
    lights.push_back(new Light({2.f, 2.f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {0.f, 0.f, 1.f}, 1.f));
}

void Scene::buildMultiMeshs() {
    auto ramMat = new Material(controller, "Ram", 1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    auto gargMat = new Material(controller, "Gargoyle", 0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    objects.push_back(new Object(groundMesh, &groundMat, "Ground"));
    materials.push_back(&groundMat);

    Mesh wallMesh;
    wallMesh.loadOFF("models/wall.off");
    wallMesh.setSquareTextureMapping();
    objects.push_back(new Object(wallMesh, &mirrorMat, "Left wall", {-1.9f, 0.f, 1.5f}));
    materials.push_back(&mirrorMat);

    wallMesh.rotate({0.f, 0.f, 1.f}, 3*M_PI/2);
    objects.push_back(new Object(wallMesh, &red, "Back wall", {0.f, 1.9f, 1.5}));
    materials.push_back(&red);

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram", {-1.f, 0.f, 0.f}, {0,-.5,0}));
    materials.push_back(ramMat);

    Mesh gargMesh;
    gargMesh.loadOFF("models/gargoyle.off");
    objects.push_back(new Object(gargMesh, gargMat, "Gargoyle", {-1.f, 1.0f, 0.f}));
    materials.push_back(gargMat);

    Mesh rhinoMesh;
    rhinoMesh.loadOFF("models/rhino.off");
    objects.push_back(new Object(rhinoMesh, &rhinoMat, "Rhino", {1.f, 0.f, 0.4f}));
    materials.push_back(&rhinoMat);

    SkyBoxMaterial *skyBoxMaterial = new SkyBoxMaterial(controller);
    objects.push_back(SkyBox::generateSkyBox(skyBoxMaterial));
    materials.push_back(skyBoxMaterial);


    lights.push_back(new Light({2.f, -3.f, 5.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildOutdor() {
    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    groundMesh.scale(5);
    groundMesh.setSquareTextureMapping();
    objects.push_back(new Object(groundMesh, &groundMat, "Ground"));
    materials.push_back(&groundMat);

    Mesh wallMesh;
    wallMesh.loadOFF("models/wall.off");
    wallMesh.setSquareTextureMapping();
    wallMesh.setSquareTextureMapping();
    objects.push_back(new Object(wallMesh, &mirrorMat, "Left wall", {-2.f, 0.f, 1.5f}));
    materials.push_back(&mirrorMat);

    Mesh rhinoMesh;
    rhinoMesh.loadOFF("models/rhino.off");
    objects.push_back(new Object(rhinoMesh, &rhinoMat, "Rhino", {1.f, 0.f, 0.4f}));
    materials.push_back(&rhinoMat);

    SkyBoxMaterial *skyBoxMaterial = new SkyBoxMaterial(controller);
    objects.push_back(SkyBox::generateSkyBox(skyBoxMaterial));
    materials.push_back(skyBoxMaterial);

    lights.push_back(new Light({9.f, 9.f, 9.f}, 5.f, {1.f, 1.f, 1.f},
                               {1.f, 1.f, .4f}, 1.f));
}

void Scene::buildPool() {
    auto pool = new Material(controller, "Pool", 1.f, 0.f, {0.f, .3f, .1f},
                             [](const Vertex & v) -> float {
                                 return min(1.f, 0.4f+Perlin(0.5f, 4, 10)(v.getPos()));
                             });

    // TODO: real pool balls ?
    const vector<Vec3Df> color = {
        {1.f, 1.f, .0f},
        {1.f, .0f, .5f}, {.3f, .3f, .8f},
        {.0f, 1.f, .0f}, {.0f, .0f, .0f}, {1.f, .8f, .2f},
        {.0f, .6f, .4f}, {.2f, .2f, 1.f}, {1.f, .0f, .0f}, {.5f, .5f, .1f},
        {.4f, .7f, .2f}, {.5f, 1.f, .5f}, {.7f, .2f, .0f}, {.0f, .0f, .3f}, {.7f, .5f, .3f}
    };

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere.off");

    const float height = Object::computeBoundingBox(sphereMesh).getHeight()/2;
    const float delta = sqrt(3.f)*height;
    groundMesh.scale(height*15);

    objects.push_back(new Object(groundMesh, pool, "Pool"));
    materials.push_back(pool);

    // Feeling lazy...
    int ballNumber = 0;
    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j <= i ; j++) {
            stringstream numberConvert;
            numberConvert<<ballNumber;
            auto ball = new Material(controller, "Ball #"+numberConvert.str(), 1.f, 1.f, color[(i*(i+1))/2+j], .1f, 50);
            materials.push_back(ball);
            objects.push_back(new Object(sphereMesh, ball, "Ball #"+numberConvert.str(), {-i*delta, (2*j-i)*height, height}));
            ballNumber++;
        }

    lights.push_back(new Light({5.f, 5.f, 20.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, .7f));
}

void Scene::buildMirrorGlass() {
    auto groundMat = new Material(controller, "Ground", 1.f, 1.f, {1.f, 1.f, 1.f}, .1f, 30);
    auto wallMat = new Material(controller, "Wall", 1.f, .5f, {.3f, .6f, .6f},
                                [](const Vertex & v) -> float {
                                    float perturbation = 0.05; // <1
                                    float f0 = 4;
                                    float lines = 30;
                                    double valeur = (1 - cos(lines * 2 * M_PI * ((v.getPos()[0]+v.getPos()[1]) / f0 + perturbation * Perlin(0.5f, 7, f0)(v.getPos())))) / 2;
                                    return valeur;
                                });
    auto ramMat = new Material(controller, "Ram", 1.f, 0.3f, Vec3Df (.7f, .4f, .2f),
                               [](const Vertex & v) -> float {
                                   return min(1.f, .3f+Perlin(0.5f, 4, 15)(v.getPos()));
                               });
    auto glassMat = new Glass(controller, "Glass", 1.4f);

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();

    objects.push_back(new Object(groundMesh, groundMat, "Ground"));
    materials.push_back(groundMat);

    groundMesh.rotate({0,1,0}, M_PI);
    objects.push_back(new Object(groundMesh, &white, "Ceiling", {0, 0, 4}));
    materials.push_back(&white);

    groundMesh.rotate({0,1,0}, M_PI/2);
    objects.push_back(new Object(groundMesh, wallMat, "Right Wall", {2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, wallMat, "Back Wall", {0, 2, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, wallMat, "Left Wall", {-2, 0, 2}));
    materials.push_back(wallMat);

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, &mirrorMat, "Mirror Wall", {0, -2, 2}));
    materials.push_back(&mirrorMat);

    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere.off");

    objects.push_back(new Object(sphereMesh, groundMat, "Pedestal"));

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram", {0.f, 0.f, .85f}));
    materials.push_back(ramMat);

    sphereMesh.scale(0.5);
    objects.push_back(new Object(sphereMesh, &mirrorMat, "Mirror1", {-1 , 2, 1}));
    objects.push_back(new Object(sphereMesh, &mirrorMat, "Mirror2", {1 , 2, 1}));
    objects.push_back(new Object(sphereMesh, &mirrorMat, "Mirror2", {0 , 2, 1.f+sqrt(3.f)}));


    auto glass = new Object(sphereMesh, glassMat, "glass", {1 , 1, 3});
    glassMat->setObject(glass);
    objects.push_back(glass);
    materials.push_back(glassMat);

    lights.push_back(new Light({-1.3f, -2.9f, 3.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, .7f));
}

unsigned int Scene::getObjectMaterialIndex(unsigned int objectIndex) const {
    const Object *object = objects[objectIndex];
    unsigned int result = 0;
    for (const Material *material : materials) {
        if (material == &object->getMaterial()) {
            return result;
        }
        result++;
    }
    return -1;
}
