// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>

#include "Object.h"
#include "Light.h"
#include "BoundingBox.h"
#include "Vec3D.h"
#include "Observable.h"
#include "Material.h"


class Controller;

class Scene: public Observable {
public:
    inline std::vector<Object *> & getObjects () { return objects; }
    inline const std::vector<Object *> & getObjects () const { return objects; }

    inline std::vector<Light *> & getLights () { return lights; }
    inline const std::vector<Light *> & getLights () const { return lights; }

    inline std::vector<Material *> &getMaterials() {return materials;}

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

    bool hasMobile() const {
        for (auto o : objects)
            if(o->isMobile())
                return true;
        return false;
    }

    void move(unsigned nbImages) {
        for (auto o : objects)
            o->move(nbImages);
    }

    void reset() {
        for (auto o : objects)
            o->reset();
    }

    Scene(Controller *, int argc, char **argv);
    virtual ~Scene ();

    /** Return the index of the material of an object, -1 if not found */
    unsigned int getObjectMaterialIndex(unsigned int objectIndex) const;

private:
    Material red, green, blue, white, black;
    Material glossyMat, groundMat, rhinoMat;
    Mirror mirrorMat;
    std::vector<Material *> materials;

    Controller *controller;

    void buildRoom(Material *sphereMat=nullptr);
    void buildMultiLights();
    void buildMultiMeshs();
    void buildPool();
    void buildOutdor();
    void buildMirrorGlass();
    void buildMesh(const std::string & path, Material *mat);
    std::vector<Object *> objects;
    std::vector<Light *> lights;
    BoundingBox bbox;
};


#endif // SCENE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
