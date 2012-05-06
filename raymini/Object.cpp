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

Vec3Df Object::getTextureColor(const Ray *intersectingRay) const {
    if (!texture || !intersectingRay->intersect()) {
        //cerr<<__FUNCTION__<<": cannot get the texture color!"<<endl;
        // Horrible pink for debug
        return Vec3Df(1, 0, 1);
    }

    const Triangle *t = intersectingRay->getTriangle();

    // TODO implement Vec2D...
    unsigned int uA = t->getU(0);
    unsigned int vA = t->getV(0);
    unsigned int uB = t->getU(1);
    unsigned int vB = t->getV(1);
    unsigned int uC = t->getU(2);
    unsigned int vC = t->getV(2);

    float interCA = intersectingRay->getU();
    float interCB = intersectingRay->getV();

    int uCA = uA - uC;
    int vCA = vA - vC;
    int uCB = uB - uC;
    int vCB = vB - vC;

    unsigned int interU = uC + uCA * interCA + uCB * interCB;
    unsigned int interV = vC + vCA * interCA + vCB * interCB;

    Vec3Df color = texture->getColor(interU, interV);
    color /= 256.0;
    return color;
}
