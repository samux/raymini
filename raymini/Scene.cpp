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
#include "NoiseUser.h"

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
         << "\toutdoor: rhino in front of a mirror + sky box" << endl
         << "\tpool : pool table" << endl
         << "\tmg : mirror and glass" << endl
         << "\tsphere : 3 spheres and grey ground (PT)" << endl
         << "\tmesh <mesh_path>" << endl
         << endl;
    exit(1);
}

Scene::Scene(Controller *c, int argc, char **argv) :
    controller(c)
{
    basicNormal = new MeshNormalTexture();
    normalTextures.push_back(basicNormal);
    swarmNormal = new ImageNormalTexture("normals/swarm.jpg", "Scales");
    normalTextures.push_back(swarmNormal);
    crossNormal = new ImageNormalTexture("normals/cross.jpg", "Cross");
    normalTextures.push_back(crossNormal);
    perlinNormal = new NoiseNormalTexture(
            NoiseUser::Predefined::PERLIN_MARBLE,
            {.6f, .6f, .7f},
            "Perlin Normal");
    normalTextures.push_back(perlinNormal);
    auto basicTexture = new DebugColorTexture();
    colorTextures.push_back(basicTexture);
    auto redTexture = new SingleColorTexture({1, 0, 0}, "Red");
    colorTextures.push_back(redTexture);
    auto greenTexture = new SingleColorTexture({0, 1, 0}, "Green");
    colorTextures.push_back(greenTexture);
    auto blueTexture = new SingleColorTexture({0, 0, 1}, "Blue");
    colorTextures.push_back(blueTexture);
    whiteTexture = new SingleColorTexture({1, 1, 1}, "White");
    colorTextures.push_back(whiteTexture);
    auto blackTexture = new SingleColorTexture({0, 0, 0}, "Black");
    colorTextures.push_back(blackTexture);
    auto greyTexture = new SingleColorTexture({.5, .5, .5}, "Grey");
    colorTextures.push_back(greyTexture);
    auto grassTexture = new ImageColorTexture("textures/grass.jpg", "Grass Texture");
    colorTextures.push_back(grassTexture);
    auto barkTexture = new ImageColorTexture("textures/bark.jpg", "Bark Texture");
    colorTextures.push_back(barkTexture);
    auto brickTexture = new ImageColorTexture("textures/brick.jpg", "Brick Texture");
    colorTextures.push_back(brickTexture);
    auto concreteTexture = new ImageColorTexture("textures/concrete.jpg", "Concrete Texture");
    colorTextures.push_back(concreteTexture);
    auto metalTexture = new ImageColorTexture("textures/metal.jpg", "Metal Texture");
    colorTextures.push_back(metalTexture);
    auto stoneTexture = new ImageColorTexture("textures/stone.jpg", "Stone Texture");
    colorTextures.push_back(stoneTexture);
    auto rhinoTexture = new NoiseColorTexture(
            {.6f, .6f, .7f},
            NoiseUser::Predefined::PERLIN_MARBLE,
            "Rhino Texture");
    colorTextures.push_back(rhinoTexture);
    auto skyBoxTexture = new ImageColorTexture(SkyBox::textureFileName, "Sky Box");
    colorTextures.push_back(skyBoxTexture);
    poolTexture = new NoiseColorTexture(
            {0.f, .3f, .1f},
            NoiseUser::Predefined::PERLIN_SPOTTED,
            "Pool");
    colorTextures.push_back(poolTexture);

    red = new Material(c, "Red", 1, 0.5, redTexture, crossNormal);
    materials.push_back(red);
    green = new Material(c, "Green", 1, 0.5, greenTexture, perlinNormal);
    materials.push_back(green);
    blue = new Material(c, "Blue", 1, 0.5, blueTexture, swarmNormal);
    materials.push_back(blue);
    white = new Material(c, "White", 1, 0.5, whiteTexture, swarmNormal);
    materials.push_back(white);
    black = new Material(c, "Black", 1, 0.5, blackTexture, basicNormal);
    materials.push_back(black);
    grey = new Material(c, "Grey", 1, 0.5, greyTexture, basicNormal);
    materials.push_back(grey);
    glossyMat = new Material(c, "Glossy", 1.f, 1.f, redTexture, basicNormal, .1f);
    materials.push_back(glossyMat);
    groundMat = new Material(c, "Ground", 1.f, 0.f, grassTexture, basicNormal);
    materials.push_back(groundMat);
    rhinoMat = new Material(c, "Rhino", 1.f, 0.2f, rhinoTexture, basicNormal);
    materials.push_back(rhinoMat);
    mirrorMat = new Mirror(c, "Mirror", basicTexture, basicNormal);
    materials.push_back(mirrorMat);
    glassMat = new Glass(c, "Glass", 1.1f, whiteTexture, swarmNormal);
    materials.push_back(glassMat);
    skyBoxMaterial = new SkyBoxMaterial(controller, "Sky Box", skyBoxTexture, basicNormal);
    materials.push_back(skyBoxMaterial);

    string id(argc>1?argv[1]:"");
    string meshPath(argc>2?argv[2]:"");

    if(!id.compare("room")) buildRoom();
    else if(!id.compare("rs")) buildRoom(red);
    else if(!id.compare("rsm")) buildRoom(mirrorMat);
    else if(!id.compare("rsglas")) buildRoom(glassMat);
    else if(!id.compare("rsglos")) buildRoom(glossyMat);
    else if(!id.compare("lights")) buildMultiLights();
    else if(!id.compare("meshs")) buildMultiMeshs();
    else if(!id.compare("outdoor")) buildOutdor();
    else if(!id.compare("pool")) buildPool();
    else if(!id.compare("mg")) buildMirrorGlass();
    else if(!id.compare("sphere")) buildSphere();
    else if(!id.compare("mesh"))
        buildMesh(meshPath, grey);
    else printUsage(argv[0]);

    updateBoundingBox();
    setChanged(OBJECT_CHANGED);
    setChanged(LIGHT_CHANGED);
    setChanged(MATERIAL_CHANGED);
    setChanged(COLOR_TEXTURE_CHANGED);
    setChanged(NORMAL_TEXTURE_CHANGED);
}

Scene::~Scene () {
    for(auto o : objects) {
        delete o;
    }

    for(auto l : lights) {
        delete l;
    }

    for(auto m : materials) {
        delete m;
    }

    for (auto t : colorTextures) {
        delete t;
    }

    for (auto t : normalTextures) {
        delete t;
    }
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
    setChanged(BOUNDING_BOX_CHANGED);
}

void Scene::buildRoom(Material *sphereMat) {
    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();

    objects.push_back(new Object(groundMesh, white, "Ground"));

    groundMesh.rotate({0,1,0}, M_PI);
    objects.push_back(new Object(groundMesh, white, "Ceiling", {0, 0, 4}));

    groundMesh.rotate({0,1,0}, M_PI/2);
    objects.push_back(new Object(groundMesh, red, "Right Wall", {2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, green, "Back Wall", {0, 2, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, blue, "Left Wall", {-2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, black, "Front Wall", {0, -2, 2}));

    if(sphereMat) {
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
    objects.push_back(new Object(mesh, mat, path));

    lights.push_back(new Light({1.f, 1.f, 1.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildMultiLights() {
    auto ramTexture = new SingleColorTexture(Vec3Df(1.f, .6f, .2f), "ML Ram");
    colorTextures.push_back(ramTexture);
    auto ramMat = new Material(controller, "Ram", 1.f, 1.f, ramTexture, basicNormal);
    materials.push_back(ramMat);

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    objects.push_back(new Object(groundMesh, white, "Ground"));

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
    auto ramTexture = new SingleColorTexture(Vec3Df(1.f, .6f, .2f), "ML Ram");
    colorTextures.push_back(ramTexture);
    auto ramMat = new Material(controller, "Ram", 1.f, 1.f, ramTexture, basicNormal);
    materials.push_back(ramMat);
    auto gargTexture = new SingleColorTexture(Vec3Df(0.5f, 0.8f, 0.5f), "ML Gargoyle");
    colorTextures.push_back(gargTexture);
    auto gargMat = new Material(controller, "Gargoyle", 0.7f, 0.4f, gargTexture, basicNormal);
    materials.push_back(gargMat);

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    // Use texture 5 times in each dimension
    groundMesh.setUVScales(5, 5);
    objects.push_back(new Object(groundMesh, groundMat, "Ground"));

    Mesh wallMesh;
    wallMesh.loadSquare();
    wallMesh.setSquareTextureMapping();
    wallMesh.rotate({0, 1, 0}, M_PI/2.0);
    wallMesh.scale(2.0*1.95251, 1);
    wallMesh.scale(3, 2);
    objects.push_back(new Object(wallMesh, mirrorMat, "Left wall", {-1.95251f, 0.f, 1.5}));

    wallMesh.rotate({0, 0, 1}, 3.0*M_PI/2.0);
    objects.push_back(new Object(wallMesh, red, "Back wall", {0.f, 1.95251f, 1.5}));

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram", {-1.f, 0.f, 0.f}, {0,-.5,0}));

    Mesh gargMesh;
    gargMesh.loadOFF("models/gargoyle.off");
    objects.push_back(new Object(gargMesh, gargMat, "Gargoyle", {-1.f, 1.0f, 0.f}));

    Mesh rhinoMesh;
    rhinoMesh.loadOFF("models/rhino.off");
    objects.push_back(new Object(rhinoMesh, rhinoMat, "Rhino", {1.f, 0.f, 0.4f}));

    Object *skybox = SkyBox::generateSkyBox(skyBoxMaterial);
    objects.push_back(skybox);

    lights.push_back(new Light({2.f, -3.f, 5.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.f));
}

void Scene::buildOutdor() {
    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    groundMesh.scale(5);
    groundMesh.setSquareTextureMapping();
    // Use texture 10 times in each dimension
    groundMesh.setUVScales(10, 10);
    objects.push_back(new Object(groundMesh, groundMat, "Ground"));

    Mesh wallMesh;
    wallMesh.loadOFF("models/wall.off");
    wallMesh.setSquareTextureMapping();
    wallMesh.setSquareTextureMapping();
    objects.push_back(new Object(wallMesh, mirrorMat, "Left wall", {-2.f, 0.f, 1.5f}));

    Mesh rhinoMesh;
    rhinoMesh.loadOFF("models/rhino.off");
    objects.push_back(new Object(rhinoMesh, rhinoMat, "Rhino", {1.f, 0.f, 0.4f}));

    Object *skybox = SkyBox::generateSkyBox(skyBoxMaterial);
    objects.push_back(skybox);

    lights.push_back(new Light({9.f, 9.f, 9.f}, 5.f, {1.f, 1.f, 1.f},
                               {1.f, 1.f, .4f}, 1.f));
}

void Scene::buildPool() {
    auto pool = new Material(controller, "Pool", 1.f, 0.f, poolTexture, basicNormal);

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

    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j <= i ; j++) {
            unsigned ballId = (i*(i+1))/2+j;
            stringstream numberConvert;
            numberConvert<<ballId;
            Vec3Df c = color[ballId];
            auto ballTexture = new SingleColorTexture(c, "Ball #"+numberConvert.str());
            colorTextures.push_back(ballTexture);
            auto ball = new Material(controller, "Ball #"+numberConvert.str(),
                                     1.f, 1.f, ballTexture,
                                     basicNormal, .1f, 50);
            materials.push_back(ball);
            objects.push_back(new Object(sphereMesh, ball, "Ball #"+numberConvert.str(), {-i*delta, (2*j-i)*height, height}));
        }

    lights.push_back(new Light({5.f, 5.f, 20.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, .7f));
}

void Scene::buildSphere() {
    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();
    groundMesh.scale(2.0);

    objects.push_back(new Object(groundMesh, grey, "Ground"));

    float rayon = 0.5;

    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere.off");
    sphereMesh.scale(rayon);
    auto sphere1 = new Object(sphereMesh, red, "Sphere1", {0, 0, rayon});
    objects.push_back(sphere1);

    auto sphere2 = new Object(sphereMesh, green, "Sphere2", {2*rayon, 0, rayon});
    objects.push_back(sphere2);

    auto sphere3 = new Object(sphereMesh, blue, "Sphere3", {-2*rayon, 0, rayon});
    objects.push_back(sphere3);

    lights.push_back(new Light({0.f, -1.f, 0.3f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.0f));

    lights.push_back(new Light({0.f, 0.f, 10.f}, 0.01, {0.f, 0.f, 1.f},
                               {1.f, 1.f, 1.f}, 1.0f));
}

void Scene::buildMirrorGlass() {
    auto groundMat = new Material(controller, "MG Ground", 1.f, 1.f, whiteTexture, basicNormal, .1f, 30);
    materials.push_back(groundMat);

    auto wallTexture = new NoiseColorTexture(
            {.3f, .6f, .6f},
            NoiseUser::Predefined::PERLIN_LINES,
            "MG Wall");
    colorTextures.push_back(wallTexture);

    auto wallNormal = new NoiseNormalTexture(
            NoiseUser::Predefined::PERLIN_LINES,
            {.3f, .6f, .6f},
            "MG Wall");
    normalTextures.push_back(wallNormal);

    auto wallMat = new Material(controller, "MG Wall", 1.f, .5f, wallTexture, wallNormal);
    materials.push_back(wallMat);

    auto ramTexture = new NoiseColorTexture(
            Vec3Df(.7f, .4f, .2f),
            NoiseUser::Predefined::PERLIN_CLOUDED,
            "MG Ram");
    colorTextures.push_back(ramTexture);

    auto ramMat = new Material(controller, "Ram", 1.f, 0.3f, ramTexture, basicNormal);
    materials.push_back(ramMat);

    auto glassMat = new Glass(controller, "Glass", 1.4f, whiteTexture, basicNormal);
    materials.push_back(glassMat);

    Mesh groundMesh;
    groundMesh.loadOFF("models/ground.off");
    groundMesh.setSquareTextureMapping();

    objects.push_back(new Object(groundMesh, groundMat, "Ground"));

    groundMesh.rotate({0,1,0}, M_PI);
    objects.push_back(new Object(groundMesh, grey, "Ceiling", {0, 0, 4}));

    groundMesh.rotate({0,1,0}, M_PI/2);
    objects.push_back(new Object(groundMesh, wallMat, "Right Wall", {2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, wallMat, "Back Wall", {0, 2, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, wallMat, "Left Wall", {-2, 0, 2}));

    groundMesh.rotate({0,0,1}, M_PI/2);
    objects.push_back(new Object(groundMesh, mirrorMat, "Mirror Wall", {0, -2, 2}));

    Mesh sphereMesh;
    sphereMesh.loadOFF("models/sphere.off");

    objects.push_back(new Object(sphereMesh, groundMat, "Pedestal"));

    Mesh ramMesh;
    ramMesh.loadOFF("models/ram.off");
    objects.push_back(new Object(ramMesh, ramMat, "Ram", {0.f, 0.f, .85f}));

    sphereMesh.scale(0.5);
    objects.push_back(new Object(sphereMesh, mirrorMat, "Mirror1", {-1 , 2, 1}));
    objects.push_back(new Object(sphereMesh, mirrorMat, "Mirror2", {1 , 2, 1}));
    objects.push_back(new Object(sphereMesh, mirrorMat, "Mirror2", {0 , 2, 1.f+sqrt(3.f)}));


    auto glass = new Object(sphereMesh, glassMat, "glass", {1 , 1, 3});
    objects.push_back(glass);

    lights.push_back(new Light({-1.9f, 0, 2.5f}, 0.5, {1.f, 0.f, 0.f},
                               {1.f, 1.f, 1.f}, .7f));
}

unsigned int Scene::getObjectMaterialIndex(unsigned int objectIndex) const {
    if (objectIndex >= objects.size()) {
        return -1;
    }
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

unsigned int Scene::getMaterialColorTextureIndex(unsigned int materialIndex) const {
    if (materialIndex >= materials.size()) {
        return -1;
    }
    const Material *material = materials[materialIndex];
    unsigned int result = 0;
    for (const ColorTexture *texture : colorTextures) {
        if (texture == material->getColorTexture()) {
            return result;
        }
        result++;
    }
    return -1;
}

unsigned int Scene::getMaterialNormalTextureIndex(unsigned int materialIndex) const {
    if (materialIndex >= materials.size()) {
        return -1;
    }
    const Material *material = materials[materialIndex];
    unsigned int result = 0;
    for (const NormalTexture *texture : normalTextures) {
        if (texture == material->getNormalTexture()) {
            return result;
        }
        result++;
    }
    return -1;
}

void Scene::updateMaterialsColorTexture(ColorTexture *oldOne, ColorTexture *newOne) {
    for (Material *m : materials) {
        if (m->getColorTexture() == oldOne) {
            m->setColorTexture(newOne);
            setChanged(MATERIAL_CHANGED);
        }
    }
}

void Scene::updateMaterialsNormalTexture(NormalTexture *oldOne, NormalTexture *newOne) {
    for (Material *m : materials) {
        if (m->getNormalTexture() == oldOne) {
            m->setNormalTexture(newOne);
            setChanged(MATERIAL_CHANGED);
        }
    }
}
