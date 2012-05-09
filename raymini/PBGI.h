#pragma once
#include "Octree.h"
#include "PointCloud.h"
#include "Ray.h"
#include "Object.h"
#include "Light.h"
#include "Observable.h"

class Controller;

class PBGI: public Observable {
public:
    static const unsigned long PBGI_CHANGED = 1<<0;

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

    void update() {
        if (cloud) {
            delete cloud;
        }
        if (octree) {
            delete octree;
        }
        cloud = new PointCloud(c);
        cloud->generatePoints();
        octree = new Octree(c, *cloud);
        setChanged(PBGI_CHANGED);
    }

private:
    Controller * c;
    unsigned int res;
    PointCloud * cloud;
    Octree * octree;
};
