// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include <QProgressDialog>

using namespace std;

static RayTracer * instance = NULL;

RayTracer * RayTracer::getInstance () {
    if (instance == NULL)
        instance = new RayTracer ();
    return instance;
}

void RayTracer::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

inline int clamp (float f, int inf, int sup) {
    int v = static_cast<int> (f);
    return (v < inf ? inf : (v > sup ? sup : v));
}

// POINT D'ENTREE DU PROJET.
// Le code suivant ray trace uniquement la boite englobante de la scene.
// Il faut remplacer ce code par une veritable raytracer
QImage RayTracer::render (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight) {
    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
    Scene * scene = Scene::getInstance ();
    const BoundingBox & bbox = scene->getBoundingBox ();
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    const Vec3Df rangeBb = maxBb - minBb;
    QProgressDialog progressDialog ("Raytracing...", "Cancel", 0, 100);
    progressDialog.show ();
    for (unsigned int i = 0; i < screenWidth; i++) {
        progressDialog.setValue ((100*i)/screenWidth);
        for (unsigned int j = 0; j < screenHeight; j++) {
            float tanX = tan (fieldOfView)*aspectRatio;
            float tanY = tan (fieldOfView);
            Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
            Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
            Vec3Df step = stepX + stepY;
            Vec3Df dir = direction + step;
            dir.normalize ();
            Vec3Df intersectionPoint;
            float smallestIntersectionDistance = 1000000.f;
            Vec3Df c (backgroundColor);
            bool found = false;
            for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
                if (found)
                    break;
                const Object & o = scene->getObjects()[k];
                Ray ray (camPos-o.getTrans (), dir);
                for(unsigned int l = 0; l < o.getMesh().getTriangles().size(); l++) {
                    const Triangle & t = o.getMesh().getTriangles() [l];
                    const Vertex & v0 = o.getMesh().getVertices() [t.getVertex(0)];
                    const Vertex & v1 = o.getMesh().getVertices() [t.getVertex(1)];
                    const Vertex & v2 = o.getMesh().getVertices() [t.getVertex(2)];
                    bool hasIntersection = ray.intersect(v0, v1, v2);
                    if (hasIntersection) {
                        c = Vec3Df(255.0, 255.0, 255.0);
                        /*float intersectionDistance = Vec3Df::squaredDistance (intersectionPoint + o.getTrans (),
                                camPos);
                        if (intersectionDistance < smallestIntersectionDistance) {
                            c = 255.f * ((intersectionPoint - minBb) / rangeBb);
                            smallestIntersectionDistance = intersectionDistance;
                        }*/
                        //cout << k << " " << l << " BIM" << endl;
                        found = true;
                        break;
                    }
                    //cout << i << " " << j << " " << k << " " << l << endl;
                }
                //bool hasIntersection = ray.intersect (o.getBoundingBox (), intersectionPoint);
            }
            image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
        }
    }
    progressDialog.setValue (100);
    return image;
}
