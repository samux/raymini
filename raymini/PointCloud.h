#pragma once

#include <vector>

#include "Surfel.h"
#include "Object.h"
#include "Scene.h"
#include "Light.h"

class Controller;

/**
 * A point cloud
 */
class PointCloud {
private:
    Controller * c;
    std::vector<Surfel> surfels;
    std::vector<Object> objects;
    float resolution;

public:
    /** Construct point cloud from the scene */
    PointCloud(Controller * c);

    ~PointCloud();

    /** Return the objects, generate them if null */
    const std::vector<Object>& getObjects(unsigned int precision=20);

    /** Return the surfels */
    const std::vector<Surfel>& getSurfels() const;

    /** Generate the points from the scene */
    void generatePoints();
private:

    /* Generate objects representing the surfels */
    void generateObjects(unsigned int precision);
};
