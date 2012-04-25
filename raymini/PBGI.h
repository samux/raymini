#pragma once
#include "Octree.h"
#include "PointCloud.h"
#include "Ray.h"
#include "Object.h"
#include "Light.h"

class PBGI {
public:
    PBGI(unsigned int res = 4) : res(res), cloud(nullptr), octree(nullptr){}
    ~PBGI(){
        delete cloud;
        delete octree;
    }
    void init();
    Octree * getOctree() const {return octree;}
    PointCloud * getPointCloud() const {return cloud;}
    std::vector<Light> getLights(const Ray & r) const;
    void setResolution(unsigned int r) {res = r;}

private:
    unsigned int res;
    PointCloud * cloud;
    Octree * octree;
};
