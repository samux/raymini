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
    inline Object () : tree(nullptr), mobile(false) {}
    Object (const Mesh & mesh, const Material * mat, bool mobile=false) :
        mesh (mesh), mat (mat), tree(nullptr), mobile(mobile) {
        updateBoundingBox ();
    }
    inline Object (const Object & o) :
        mesh (o.mesh), mat (o.mat), bbox (o.bbox), trans (o.trans), origTrans(trans), tree(nullptr), mobile(o.mobile) {}
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
        return (*this);
    }

    inline const Vec3Df & getTrans () const { return trans;}
    inline void setTrans (const Vec3Df & t) {
        trans = t;
        origTrans = trans;
    }

    inline void move(const Vec3Df & v) { if(mobile) trans += v; }
    inline void reset() { trans = origTrans; }

    inline bool isMobile() const {return mobile; }
    inline void setMobile(bool mobile) { this->mobile = mobile; }

    inline const Mesh & getMesh () const { return mesh; }
    inline Mesh & getMesh () { return mesh; }

    inline const Material & getMaterial () const { return *mat; }

    inline const KDtree & getKDtree () {
        if(!tree)
            tree = new KDtree(*this);
        return *tree;
    }

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox ();

private:
    Mesh mesh;
    const Material * mat;
    BoundingBox bbox;
    Vec3Df trans;
    Vec3Df origTrans;
    KDtree *tree;
    bool mobile;
};

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
