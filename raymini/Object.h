// *********************************************************
// Object Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#pragma once

#include <iostream>
#include <vector>

#include "Mesh.h"
#include "Material.h"
#include "BoundingBox.h"
#include "KDtree.h"
#include "Brdf.h"
#include "Light.h"

class Object {
public:
    inline Object () : tree(nullptr) {}
    Object (const Mesh & mesh, const Material * mat) :
        mesh (mesh), mat (mat), tree(nullptr), enabled(true) {
        updateBoundingBox ();
    }
    inline Object (const Object & o) :
        mesh (o.mesh), mat (o.mat), bbox (o.bbox), trans (o.trans), origTrans(trans), tree(nullptr), mobile(o.mobile), enabled(o.enabled) {}
    virtual ~Object () {
        delete tree;
    }

    inline Object & operator= (const Object & o) {
        mesh = o.mesh;
        mat = o.mat;
        bbox = o.bbox;
        trans = o.trans;
        origTrans = trans;
        tree = nullptr;
        mobile = o.mobile;
        enabled = o.enabled;
        return (*this);
    }

    inline const Vec3Df & getTrans () const { return trans;}
    inline void setTrans (const Vec3Df & t) {
        trans = t;
        origTrans = trans;
    }

    inline void move(unsigned nbImages) { trans += mobile/nbImages ; }
    inline void reset() { trans = origTrans; }

    inline bool isMobile() const {return mobile!=Vec3Df(); }
    inline void setMobile(const Vec3Df & mobile) { this->mobile = mobile; }

    inline const Mesh & getMesh () const { return mesh; }
    inline Mesh & getMesh () { return mesh; }

    inline const Material & getMaterial () const { return *mat; }

    inline const KDtree & getKDtree () {
        if(!tree)
            tree = new KDtree(*this);
        return *tree;
    }

    inline void setEnabled(bool e) { enabled = e; }
    inline bool isEnabled() const { return enabled; }

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

private:
    Mesh mesh;
    const Material * mat;
    BoundingBox bbox;
    Vec3Df trans;
    Vec3Df origTrans;
    KDtree *tree;
    Vec3Df mobile;
    bool enabled;
};

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
