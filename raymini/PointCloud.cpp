#include "PointCloud.h"

#include "Vertex.h"
#include "Triangle.h"
#include "Light.h"
#include "Brdf.h"
#include "Material.h"
#include "RayTracer.h"
#include "Controller.h"
#include "SkyBox.h"

using namespace std;

PointCloud::PointCloud(Controller * c): c(c), resolution(256) {}


PointCloud::~PointCloud() {
    for (Object *o:objects) {
        delete o;
    }
}

void PointCloud::generatePoints() {
    const Scene * scene = c->getScene();
    const RayTracer *rayTracer = c->getRayTracer();

    surfels.clear();

    // For each light
    for (const Light * light : scene->getLights()) {
        Vertex v(light->getPos(), light->getNormal());
        vector<Vec3Df> directions = v.getDirectionsOnCube(resolution);
        Vec3Df position = light->getPos();
        // For each pixel
        for (const Vec3Df &direction : directions) {
            Ray bestRay;

            // We add the surfel
            if (rayTracer->intersect(direction, position, bestRay)) {
                Object *object = bestRay.getIntersectedObject();
                const Material & mat = object->getMaterial();
                Vertex intersection = bestRay.getIntersection();
                Vec3Df intPos = intersection.getPos();
                Vec3Df intNorm = intersection.getNormal();
                float distance = sqrt(bestRay.getIntersectionDistance());
                float pixelDistance = direction.getLength();
                Vec3Df normalizedDirection(direction);
                normalizedDirection.normalize();
                float radius = (1.0+abs(Vec3Df::crossProduct(normalizedDirection, intNorm).getLength()))*distance/(pixelDistance*resolution);
                auto isSkyBox = dynamic_cast<const SkyBox*>(object);
                if(!mat.isGlossy() && !isSkyBox) {
                    vector<Light> singleLight({*light});
                    Vec3Df color = mat.genColor(
                            position,
                            &bestRay,
                            singleLight,
                            Brdf::Diffuse);
                    Surfel surfel(
                            intPos,
                            intNorm,
                            radius,
                            color,
                            &mat);
                    surfels.push_back(surfel);
                }
            }
        }
    }
}

void PointCloud::generateObjects(unsigned int precision) {
    objects.resize(surfels.size());
    unsigned int index = 0;
    for (const Surfel & surfel : surfels) {
        objects[index] = surfel.generateObject(precision);
        index++;
    }
}

const vector<Object*>& PointCloud::getObjects(unsigned int precision) {
    if (objects.empty()) {
        generateObjects(precision);
    }
    return objects;
}

const vector<Surfel>& PointCloud::getSurfels() const {
    return surfels;
}
