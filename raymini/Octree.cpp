#include "Octree.h"
#include "Object.h"
#include "PointCloud.h"
#include "Scene.h"
#include "Surfel.h"
#include "Controller.h"

using namespace std;

Octree::Octree(Controller * c, const PointCloud & cloud) : c(c), cloud(cloud) {
    Scene * sc = c->getScene();
    bBox = sc->getBoundingBox();

    surfels.resize(cloud.getSurfels().size());
    for(int i = 0; i < 8; i++)
        sons[i] = nullptr;
    for(unsigned int i = 0 ; i < cloud.getSurfels().size() ; i++)
        surfels[i] = i;
    next();
}

void Octree::next() {
    if(surfels.size() <= MIN_SURFELS) return;//leaf

    array<BoundingBox, 8> s;
    bBox.subdivide(s);

    array<vector<unsigned>, 8> array_surfels;
    splitSurfels(s, array_surfels);
    
    surfels.clear(); //not a leaf

    for(unsigned int index = 0; index < 8; index++) {
        sons[index] = new Octree(c, cloud, array_surfels[index], s[index]);
    }
}

void Octree::exec(void (*f)(const Octree * octree)) const {
    f(this);
    if(!isLeaf()) {
        for(unsigned int i = 0; i < 8; i++) {
            (sons[i])->exec(f);
        }
    }
}

void Octree::splitSurfels(const array<BoundingBox, 8> & bBoxes, array<vector<unsigned>, 8> & array_surfels) {

    for(unsigned surfel : surfels) {
        const Vec3Df & p = cloud.getSurfels()[surfel].getPos();
        for(unsigned int j = 0; j < 8; j++) {
            if(bBoxes[j].contains(p)) {
                array_surfels[j].push_back(surfel);
            }
        }
    }
}

Surfel Octree::getMeanSurfel() const {
    Vec3Df p, n, color;
    float radius (0.0);
    if(isLeaf()) {
        for(unsigned index_surfel: surfels) {
            Surfel s = cloud.getSurfels() [index_surfel];
            p += s.getPos();
            n += s.getNormal();
            radius += s.getRadius();
            color += s.getColor();
        }
        n.normalize();
        return Surfel(p/surfels.size(), n, radius/surfels.size(), color/surfels.size(), new Material(c, 1.0f, 0.0f, color/255.0));
    }
    for(unsigned int i = 0; i < 8; i++) {
        Surfel s = sons[i]->getMeanSurfel();
        p += s.getPos();
        n += s.getNormal();
        radius += s.getRadius();
        color += s.getColor();
    }
    n.normalize();
    return Surfel(p/8.0, n, radius/8.0, color/8.0, new Material(c, 1.0f, 0.0f, color/255.0));
}

bool Octree::sort_octree(pair<float, bool> p1, pair<float, bool> p2) {
    return (p1.first < p2.first);
}

const Octree * Octree::intersect(Ray &ray) const {

    if(isLeaf()) {
        for(unsigned index_surfel : surfels) {
            Surfel s = cloud.getSurfels()[index_surfel];
            if(Vec3Df::dotProduct(s.getNormal(), ray.getDirection()) < 0.0) {
                if(ray.intersectDisc(s.getPos(), s.getNormal(), s.getRadius())) {
                    return this;
                }
            }
        }
        return nullptr;
    }
    else {
        array<pair<float, unsigned int>, 8> pair_intersection;
        array<Vec3Df, 8> inter_pos;
        array<bool, 8> is_intersected;

        for(unsigned int i = 0; i < 8; i++) {
            pair_intersection[i].second = i;
            is_intersected[i] = ray.intersect(sons[i]->bBox, inter_pos[i]);
            pair_intersection[i].first = Vec3Df::squaredDistance (inter_pos[i], ray.getOrigin());
        }

        sort(pair_intersection.begin(), pair_intersection.end(), Octree::sort_octree);

        for(unsigned int i = 0; i < 8; i++) {
            const Octree * octree (nullptr);
            if(is_intersected[pair_intersection[i].second]) {
                if((octree = sons[pair_intersection[i].second]->intersect(ray)) != nullptr)
                    return octree;
            }
        }

        return nullptr;
    }
}

