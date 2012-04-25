#include <algorithm>

#include "Brdf.h"

using namespace std;

Vec3Df Brdf::ambient() const {
    return colorAmbient * Ka;
}

Vec3Df Brdf::lambert(Vec3Df i, Vec3Df n) const {
    return colorDif * Kd * max(Vec3Df::dotProduct(i,n),0.0f);
}

Vec3Df Brdf::phong(Vec3Df r, Vec3Df i, Vec3Df n) const {
    Vec3Df ref = 2*Vec3Df::dotProduct(n,i)*n - i;
    ref.normalize();
    float res = Ks * pow(max(Vec3Df::dotProduct(ref,r),0.0f), alpha);
    return {res, res, res};
}

Vec3Df Brdf::operator()(const Vec3Df &p, const Vec3Df &n,
                        const Vec3Df posCam, Type type) const{
    Vec3Df color;

    Vec3Df ra=(posCam-p);
    ra.normalize();

    for(const auto light : lights) {
        Vec3Df currentColor;
        Vec3Df ir=(light.getPos() - p);
        ir.normalize();

        if(type&Lambert)
            currentColor += lambert(ir, n);
        if(type&Phong)
            currentColor += phong(ra, ir, n);

        color += light.getIntensity()*light.getColor()*currentColor;
    }

    if(lights.size())
        color /= lights.size();

    if(type&Ambient)
        color += ambient();

    return color;
}
