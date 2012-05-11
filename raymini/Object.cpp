// *********************************************************
// Object Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Object.h"

#include "Ray.h"

using namespace std;

BoundingBox Object::computeBoundingBox(const Mesh & mesh) {
    BoundingBox bbox;
    const vector<Vertex> & V = mesh.getVertices ();
    if (V.empty ())
        bbox = BoundingBox ();
    else {
        bbox = BoundingBox (V[0].getPos ());
        for (unsigned int i = 1; i < V.size (); i++)
            bbox.extendTo (V[i].getPos ());
    }
    return bbox;
}

void Object::updateKDtree() {
    updateBoundingBox();
    if (tree) {
        delete tree;
    }
    tree = new KDtree(*this);
}

SkyBox *SkyBox::generateSkyBox(const SkyBoxMaterial *m, string name) {
    Mesh mesh;
    mesh.loadCube();
    mesh.returnAllTriangles();
    mesh.scale(20);
    mesh.setCubeTextureMapping(m, 2, 2);
    SkyBox *skybox = new SkyBox(mesh, m, name);
    return skybox;
}
