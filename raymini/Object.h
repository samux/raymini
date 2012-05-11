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
#include "BoundingBox.h"
#include "KDtree.h"
#include "Brdf.h"
#include "Light.h"
#include "Material.h"
#include "NamedClass.h"

class Ray;

class Object: public NamedClass {
public:
    Object(const Mesh & mesh, const Material * mat, std::string name="No name",
           const Vec3Df &trans=Vec3Df(), const Vec3Df &mobile=Vec3Df()):
        NamedClass(name),
        mesh (mesh), mat (mat), trans(trans), origTrans(trans),
        tree(nullptr), mobile(mobile), enabled(true) {
        updateBoundingBox ();
        tree = new KDtree(*this);
    }

    virtual ~Object () {
        delete tree;
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
    inline Vec3Df getMobile() const {return mobile;}

    inline const Mesh & getMesh () const { return mesh; }
    inline Mesh & getMesh () { return mesh; }

    inline const Material & getMaterial () const { return *mat; }
    inline void setMaterial(const Material *material) {mat = material;}

    inline const KDtree & getKDtree () const { return *tree; }

    inline void setEnabled(bool e) { enabled = e; }
    inline bool isEnabled() const { return enabled; }

    inline const BoundingBox & getBoundingBox () const { return bbox; }
    void updateBoundingBox () { bbox = computeBoundingBox(mesh); }
    static BoundingBox computeBoundingBox(const Mesh & mesh);

    void updateKDtree();

protected:
    Mesh mesh;
    const Material * mat;

private:
    BoundingBox bbox;
    Vec3Df trans;
    Vec3Df origTrans;
    KDtree *tree;
    Vec3Df mobile;
    bool enabled;
};

/**
 * Skybox object 
 */
class SkyBox: public Object {

public:
    virtual ~SkyBox() {};

    static constexpr const char *textureFileName = "textures/skybox.jpg";

    static SkyBox *generateSkyBox(const SkyBoxMaterial *, std::string name="SkyBox");

private:
    SkyBox(const Mesh &m, const SkyBoxMaterial *mat, std::string name):
        Object(m, mat, name) {}
};
