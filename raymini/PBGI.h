#pragma once
#include "Octree.h"
#include "PointCloud.h"
#include "Ray.h"
#include "Object.h"
#include "Light.h"

class Controller;

class PBGI {
public:
    PBGI(Controller * c, unsigned int res = 6) : c(c), res(res){
        cloud = new PointCloud(c);
        cloud->generatePoints();
        octree = new Octree(c, *cloud);
    }
    ~PBGI(){
        delete cloud;
        delete octree;
    }

    Octree * getOctree() const {return octree;}
    PointCloud * getPointCloud() const {return cloud;}
    std::vector<Light> getLights(Ray & r) const;
    void setResolution(unsigned int r) {res = r;}

private:
    Controller * c;
    unsigned int res;
    PointCloud * cloud;
    Octree * octree;
};
