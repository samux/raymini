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

#include "Brdf.h"

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
    vector<Vec3Df> posLight;
    for(unsigned int i = 0; i < scene->getLights().size(); i++)
        posLight.push_back(scene->getLights()[i].getPos());
    Brdf brdf(posLight, Vec3Df(0,0,0), Vec3Df(1.0,1.0,1.0), Vec3Df(0.5,0.5,0.0), 1.0, 1.0, 0.1, 1.5);
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
            Vertex intersection;
            float smallestIntersectionDistance = 1000000.f;
            Vec3Df c (backgroundColor);
            for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
                const Object & o = scene->getObjects()[k];
                brdf.colorDif = o.getMaterial().getColor();
                brdf.Kd = o.getMaterial().getDiffuse();
                brdf.Ks = o.getMaterial().getSpecular();
                Ray ray (camPos-o.getTrans (), dir);
                //bool hasIntersection = ray.intersect (o.getBoundingBox (), intersectionPoint);
                for(unsigned int l = 0; l < o.getMesh().getTriangles().size(); l++) {
                    const Triangle & t = o.getMesh().getTriangles() [l];
                    const Vertex & v0 = o.getMesh().getVertices() [t.getVertex(0)];
                    const Vertex & v1 = o.getMesh().getVertices() [t.getVertex(1)];
                    const Vertex & v2 = o.getMesh().getVertices() [t.getVertex(2)];
                    bool hasIntersection = ray.intersect(v0, v1, v2, intersection);
                    if (hasIntersection) {
                        float intersectionDistance = Vec3Df::squaredDistance (intersection.getPos() + o.getTrans (), camPos);
                        if(intersectionDistance < smallestIntersectionDistance) {
                            //c = 255.f * ((intersectionPoint - minBb) / rangeBb);
                            c = brdf.getColor(intersection.getPos(), intersection.getNormal(), camPos) * 255.0;
                            smallestIntersectionDistance = intersectionDistance;
                        }
                    }
                }
            }
            image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
        }
    }
    progressDialog.setValue (100);
    return image;
}
