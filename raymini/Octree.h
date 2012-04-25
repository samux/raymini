#pragma once

#include <vector>
#include <array>
#include "Vec3D.h"
#include "BoundingBox.h"
#include "Ray.h"

class PointCloud;
class Surfel;

class Octree {
protected:
    const PointCloud & cloud;
    std::vector<unsigned> surfels;// sth only if leaf;
    std::array<Octree *, 8> sons;

public:
    static const unsigned MIN_SURFELS = 16;
    BoundingBox bBox;

    Octree(const PointCloud &p);

    ~Octree() {
        for(Octree * & o:sons) {
            if(o != nullptr)
                delete o;
        }
    }

    const std::array<Octree*, 8> getSons() const { return sons; }
    const std::vector<unsigned> &  getSurfels() const { return surfels; }
    bool isLeaf() const {return (sons[0] == nullptr);}
    Surfel getMeanSurfel() const; 
    const Octree * intersect(Ray &ray) const;
    static bool sort_octree(std::pair<float, bool> p1, std::pair<float, bool> p2);
    
    // useful to draw an Octree
    void exec(void (*f)(const Octree * octree)) const;

private:
    Octree(const PointCloud & cloud, const std::vector<unsigned> & surfels,
           const BoundingBox & b):
        cloud(cloud), surfels(surfels),
        bBox(b) {
        for(int i = 0; i < 8; i++)
            sons[i] = nullptr;
        next();
    }

    Octree & operator=(const Octree &t) = delete;

    void next();
    void splitSurfels(const std::array<BoundingBox, 8> & bBoxes, std::array<std::vector<unsigned>, 8> & t);

};
