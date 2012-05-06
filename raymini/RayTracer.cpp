// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <QImage>
#include <iostream>
#include <algorithm>

#include "Controller.h"
#include "ProgressBar.h"
#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include "Color.h"
#include "Brdf.h"

using namespace std;

inline int clamp (float f) {
    int v = static_cast<int> (255*f);
    return min(max(v, 0), 255);
}

RayTracer::RayTracer(Controller *c):
    mode(Mode::RAY_TRACING_MODE),
    depthPathTracing(0), nbRayPathTracing(50), maxAnglePathTracing(M_PI/2.f),
    intensityPathTracing(3.f), onlyPathTracing(false),
    radiusAmbientOcclusion(2), nbRayAmbientOcclusion(0), maxAngleAmbientOcclusion(M_PI/3),
    intensityAmbientOcclusion(1/5.f), onlyAmbientOcclusion(false),
    typeAntiAliasing(AntiAliasing::NONE), nbRayAntiAliasing(4),
    typeFocus(Focus::NONE), nbRayFocus(9), apertureFocus(0.1),
    nbPictures(1),
    qualityDivider(1),
    quality(OPTIMAL),
    durtiestQualityDivider(5),
    durtiestQuality(ONE_OVER_X),
    controller(c),
    backgroundColor(Vec3Df(.1f, .1f, .3f)),
    shadow(this)
{}

QImage RayTracer::RayTracer::render (const Vec3Df & camPos,
                                     const Vec3Df & direction,
                                     const Vec3Df & upVector,
                                     const Vec3Df & rightVector,
                                     float fieldOfView,
                                     float aspectRatio,
                                     unsigned int screenWidth,
                                     unsigned int screenHeight) {
    Scene *scene = controller->getScene();
    vector<Color> buffer;
    qualityDivider = quality==ONE_OVER_X?qualityDivider:1;
    // To avoid black pixels on the top of the screen
    unsigned int computedScreenWidth = ceil((float)screenWidth/(float)qualityDivider);
    unsigned int computedScreenHeight = ceil((float)screenHeight/(float)qualityDivider);
    buffer.resize(computedScreenHeight*computedScreenWidth);

    vector<pair<float, float>> singleNulOffset;
    singleNulOffset.push_back(pair<float, float>(0, 0));
    const vector<pair<float, float>> offsets = quality==OPTIMAL?AntiAliasing::generateOffsets(typeAntiAliasing, nbRayAntiAliasing):singleNulOffset;
    const vector<pair<float, float>> offsets_focus = Focus::generateOffsets(typeFocus, apertureFocus, nbRayFocus);

    const float tang = tan (fieldOfView);
    const Vec3Df rightVec = tang * aspectRatio * rightVector / computedScreenWidth;
    const Vec3Df upVec = tang * upVector / computedScreenHeight;

    const Vec3Df camToObject = controller->getWindowModel()->getFocusPoint().getPos() - camPos;
    const float focalDistance = Vec3Df::dotProduct(camToObject, direction) - distanceOrthogonalCameraScreen;

    const unsigned nbIterations = scene->hasMobile()&&quality==OPTIMAL?nbPictures:1;
    ProgressBar progressBar(controller, nbIterations*computedScreenWidth);

    // For each picture
    for (unsigned picNumber = 0 ; picNumber < nbIterations; picNumber++) {

        // For each pixel
        #pragma omp parallel for
        for (unsigned int i = 0; i < computedScreenWidth; i++) {
            progressBar();
            for (unsigned int j = 0; j < computedScreenHeight && !controller->getRenderThread()->isEmergencyStop(); j++) {
                buffer[j*computedScreenWidth+i] += computePixel(camPos,
                                                                direction,
                                                                upVec, rightVec,
                                                                computedScreenWidth, computedScreenHeight,
                                                                offsets, offsets_focus,
                                                                focalDistance,
                                                                i, j);
            }
        }
        scene->move(nbPictures);
    }

    QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
    for (unsigned int i = 0; i < screenWidth; i++) {
        unsigned int computedI = i/qualityDivider;
        for (unsigned int j = 0; j < screenHeight; j++) {
            unsigned int computedJ = j/qualityDivider;
            Color c = buffer[computedJ*computedScreenWidth+computedI];
            image.setPixel(i, j, qRgb(clamp(c[0]), clamp(c[1]), clamp(c[2])));
        }
    }

    scene->reset();

    return image;
}

Vec3Df RayTracer::computePixel(const Vec3Df & camPos,
                               const Vec3Df & direction,
                               const Vec3Df & upVec,
                               const Vec3Df & rightVec,
                               unsigned int screenWidth,
                               unsigned int screenHeight,
                               const vector<pair<float, float>> &offsets,
                               const vector<pair<float, float>> &offsets_focus,
                               float focalDistance,
                               unsigned i, unsigned j) {
    Color c;

    // For each ray in each pixel
    for (const pair<float, float> &offset : offsets) {
        Vec3Df stepX = (float(i)+offset.first - screenWidth/2.f) * rightVec;
        Vec3Df stepY = (float(j)+offset.second - screenHeight/2.f) * upVec;
        Vec3Df step = stepX + stepY;
        Vec3Df dir = direction + step;
        dir.normalize();
        if (typeFocus != Focus::NONE && quality == OPTIMAL) {
            float distanceCameraScreen = sqrt(step.getLength()*step.getLength() +
                                              distanceOrthogonalCameraScreen*distanceOrthogonalCameraScreen);
            dir.normalize ();
            Vec3Df customFocalPoint = camPos + (distanceCameraScreen*(distanceOrthogonalCameraScreen + focalDistance)/
                                                distanceOrthogonalCameraScreen)*dir;
            for (const pair<float, float> &offset_focus : offsets_focus) {
                Vec3Df focusMovedCamPos = camPos + Vec3Df(1,0,0)*offset_focus.first + Vec3Df(0,1,0)*offset_focus.second;
                dir = customFocalPoint - focusMovedCamPos;
                dir.normalize();
                c += getColor(dir, focusMovedCamPos);
            }
        }
        else {
            c += getColor(dir, camPos);
        }
    }
    return c();
}

bool RayTracer::intersect(const Vec3Df & dir,
                          const Vec3Df & camPos,
                          Ray & bestRay,
                          const Object* & intersectedObject) const {
    Scene * scene = controller->getScene();
    bestRay = Ray();


    for (const Object * o : scene->getObjects()) {
        if (!o->isEnabled()) {
            continue;
        }
        Ray ray (camPos - o->getTrans ()+ DISTANCE_MIN_INTERSECT*dir, dir);

        if (o->getKDtree().intersect(ray) &&
            ray.getIntersectionDistance() < bestRay.getIntersectionDistance()) {
            intersectedObject = o;
            bestRay = ray;
        }
    }

    if(bestRay.intersect()) {
        bestRay.translate(intersectedObject->getTrans());
    }

    return bestRay.intersect();
}

Vec3Df RayTracer::getColor(const Vec3Df & dir, const Vec3Df & camPos, bool pathTracing) const {
    Ray bestRay;
    Brdf::Type type = onlyAmbientOcclusion?Brdf::Ambient:Brdf::All;
    bool useRayTracing = (quality==OPTIMAL) && pathTracing;
    return getColor(dir, camPos, bestRay, useRayTracing?0:depthPathTracing, type);
}

Vec3Df RayTracer::getColor(const Vec3Df & dir, const Vec3Df & camPos, Ray & bestRay, unsigned depth, Brdf::Type type) const {
    const Object *intersectedObject;

    if(intersect(dir, camPos, bestRay, intersectedObject)) {
        const Material & mat = intersectedObject->getMaterial();
        const vector<Light> & light = getLights(bestRay.getIntersection());

        Color color = mat.genColor(camPos,
                                   &bestRay,
                                   light,
                                   type);

        if((depth < depthPathTracing) || (mode == PBGI_MODE && quality == OPTIMAL)) {

            vector<Light> lights;
            switch(mode) {
                case RAY_TRACING_MODE:
                    lights = getLightsPT(bestRay.getIntersection(), depth);
                    break;
                case PBGI_MODE:
                    lights = controller->getPBGI()->getLights(bestRay);
                    break;
            }

            Color ptColor = mat.genColor(camPos, &bestRay,
                                         lights,
                                         Brdf::Diffuse);

            if(onlyPathTracing && depth == 0)
                color = ptColor;
            else
                color += ptColor;
        }
        return color();
    }

    return backgroundColor;
}

vector<Light> RayTracer::getLights(const Vertex & closestIntersection) const {
    vector<Light *> lights = controller->getScene()->getLights();
    vector<Light> enabledLights;

    for(Light * light : lights) {
        if (!light->isEnabled()) {
            continue;
        }
        float visibility = shadow(closestIntersection.getPos(), *light);
        Light l = *light;
        l.setIntensity(light->getIntensity()*visibility);
        enabledLights.push_back(l);
    }

    return enabledLights;
}

vector<Light> RayTracer::getLightsPT(const Vertex & closestIntersection, unsigned depth) const {
    vector<Light> lights;

    Vec3Df pos = closestIntersection.getPos();
    vector<Vec3Df> dirs = closestIntersection.getNormal().randRotate(maxAnglePathTracing,
                                                                     nbRayPathTracing);

    for (const Vec3Df & dir : dirs) {
        Ray bestRay;
        Vec3Df color = getColor(dir, pos, bestRay, depth+1, Brdf::Diffuse);

        if(bestRay.intersect()) {
            float d = bestRay.getIntersectionDistance();
            float intensity = intensityPathTracing / pow(1+d,3);

            lights.push_back(Light(bestRay.getIntersection().getPos(),
                                   color, intensity));
        }
    }
    return lights;
}

float RayTracer::getAmbientOcclusion(Vertex intersection) const {
    if ((!nbRayAmbientOcclusion)||(quality!=OPTIMAL)) return intensityAmbientOcclusion;

    int occlusion = 0;
    vector<Vec3Df> directions = intersection.getNormal().randRotate(maxAngleAmbientOcclusion,
                                                                    nbRayAmbientOcclusion);
    for (Vec3Df & direction : directions) {
        const Vec3Df & pos = intersection.getPos();

        const Object *intersectedObject;
        Ray bestRay;
        if (intersect(direction, pos, bestRay, intersectedObject)) {
            if (bestRay.getIntersectionDistance() < radiusAmbientOcclusion) {
                occlusion++;
            }
        }
    }

    return intensityAmbientOcclusion * (1.f-float(occlusion)/float(nbRayAmbientOcclusion));
}

QString RayTracer::qualityToString(Quality quality, int qualityDivider) {
    switch (quality) {
    case OPTIMAL:
        return QString("optimal");
    case BASIC:
        return QString("basic");
    case ONE_OVER_X:
        QString number;
        number = QString("%1").arg(qualityDivider*qualityDivider);
        return QString("one over ")+number+QString(" pixels");
    }
    return QString();
}
