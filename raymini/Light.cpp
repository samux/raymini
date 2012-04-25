// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Light.h"

unsigned int Light::NB_IMPULSE = 10;

std::vector<Vec3Df> Light::generateImpulsion() const{
    std::vector<Vec3Df> impulsion;
    impulsion.resize(NB_IMPULSE);
    for(unsigned int i = 0; i < NB_IMPULSE; i++) {
        Vec3Df r(rand(), rand(), rand());
        r.normalize();
        r = r.projectOn(normal);
        float norm = rand()/float(RAND_MAX);
        r = radius*norm*r;
        impulsion[i] = pos + r;
    }
    return impulsion;
}
