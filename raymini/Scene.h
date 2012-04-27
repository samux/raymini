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

class Scene {
public:
    static Scene * getInstance ();
    static void destroyInstance ();

    inline std::vector<Object> & getObjects () { return objects; }
    inline const std::vector<Object> & getObjects () const { return objects; }

    inline std::vector<Light> & getLights () { return lights; }
    inline const std::vector<Light> & getLights () const { return lights; }

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

    bool hasMobile() const {
        for (const Object &o : objects)
            if(o.isMobile())
                return true;
        return false;
    }

    void move(unsigned nbImages) {
        for (Object &o : objects)
            o.move(nbImages);
    }

    void reset() {
        for (Object &o : objects)
            o.reset();
    }

protected:
    Scene ();
    virtual ~Scene ();

private:
    void buildDefaultScene ();
    std::vector<Object> objects;
    std::vector<Light> lights;
    BoundingBox bbox;
};


#endif // SCENE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
