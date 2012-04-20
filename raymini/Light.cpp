// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Light.h"

unsigned int Light::NB_IMPULSE = 10;

std::vector<Vec3Df> Light::generateImpulsion() {
    impulsion.clear();
    for(unsigned int i = 0; i < NB_IMPULSE; i++) {
        Vec3Df r(rand(), rand(), rand());
        r.normalize();
        r = r - Vec3Df::dotProduct(r, normal)*normal;
        float norm = rand()/float(RAND_MAX);
        r = radius*norm*r;
        impulsion.push_back(pos + r);
    }
    return impulsion;
}
