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

static const float cst[9]={0,
                           0,
                           0,
                           0.3f,// IND_RUG rugosité du materiaux entre 0 & 1
                           1.5f,// IND_REF indice de refraction, toujours superieur a 1
                           2.0f,//IND_ATT indice d'atténuation a travers le milieu, typiquement nul pour les diélectrique, entre 2 et 10 pour les metaux
                           0.2f,// SIG Rugosité <0.2
                           0.01f,// SIG_Y Rugosité <0.2
                           0.5f};// K_ISO [0,1]


Vec3Df Brdf::blinnPhong(Vec3Df r, Vec3Df i, Vec3Df n) const {
    Vec3Df H=r+i;
    H.normalize();
    float coeff=Ks*pow(max(Vec3Df::dotProduct(n,H),0.f),alpha);

    return {coeff, coeff, coeff};
}

pair<Vec3Df, Vec3Df> Brdf::schlick(Vec3Df r, Vec3Df i, Vec3Df n) const {
    float K=Kd, P=cst[8],R=cst[3];
    //faire varier les 3constantes: k symbolise le facteur de reflexion, R le coefficien de ruggosité (r=0 parfaitement spéculaire et r=1 parfaitement diffus.)et P le facteur d'isotropie ( p=0 symbolise que le matériau est parfaitement anisotrope et p=1 parfaitement isotrope).
    Vec3Df ref = -i-2*Vec3Df::dotProduct(n,-i)*n;
    Vec3Df y = i+r;
    y.normalize();
    Vec3Df h = y-Vec3Df::dotProduct(y,n)*n;
    h.normalize();
    Vec3Df tt= Vec3Df::crossProduct(n,i);
    float u = max(Vec3Df::dotProduct(y,ref),0.f);
    float t = max(Vec3Df::dotProduct(y,n),0.f);
    float w = max(Vec3Df::dotProduct(tt,h),0.f);
    float s,z,a;
    float specu,diffu;

    s = K + (1-K)*pow((1-u),5);
    z = R/pow((1+R*pow(t,2)-pow(t,2)),2);
    a = sqrt(P/(pow(P,2)-pow(P,2)*pow(w,2)+pow(w,2)));
    float v = max(Vec3Df::dotProduct(i,n),0.f);
    float v1 = max(Vec3Df::dotProduct(ref,n),0.f);

    if(v1==0.0f||v==0.0f) {
        specu=0;
        diffu=0;
    }
    else {
        float c = max(Vec3Df::dotProduct(i,n),0.f);
        float b = max(Vec3Df::dotProduct(n,r),0.f);
        float g = c/(R-R*c+c);
        float g1 = b/(R-R*b+b);
        float d = ((g*g1*a*z)+(1-g*g1))/(4*v1*v);

        specu = (s*g*g1*a*z)/(4*v1*v);
        diffu = s*(4*v1*v);
    }
    return make_pair(diffu*colorDif, Vec3Df(specu, specu, specu));
}


Vec3Df Brdf::cookTorrance(Vec3Df r, Vec3Df i, Vec3Df n) const {
    float ks=1-Kd;  //coefficient de specularité
    float m=cst[3];     // rugosité du materiaux entre 0 & 1
    float indice = cst[4];  //indice de refraction, toujours superieur a 1
    float k = cst[5];   //indice d'atténuation a travers le milieu, typiquement nul pour les diélectrique, entre 2 et 10 pour les metaux

    Vec3Df H=r+i;
    H.normalize();
    float alpha =acos(Vec3Df::dotProduct(n,H));

    float F =((indice-1)*(indice-1) + 4*indice*pow((1-cos(alpha)),5)+k*k)/((indice+1)*(indice+1)+k*k);
    float D =(1/(4*m*m*pow(cos(alpha),4)))*exp(-(tan(alpha)*tan(alpha))/(m*m));
    float G =min(1.f,min(2*Vec3Df::dotProduct(n,H)*Vec3Df::dotProduct(r,n)/Vec3Df::dotProduct(r,H),2*Vec3Df::dotProduct(n,H)*Vec3Df::dotProduct(i,n)/Vec3Df::dotProduct(r,H)));
    float spec = ks*F*D*G;

    return {spec, spec, spec};
}

Vec3Df Brdf::ward(Vec3Df r, Vec3Df i, Vec3Df n) const {
    float sig=cst[6];// Rugosité <0.2
    Vec3Df H=r+i;
    H.normalize();
    float spec=Vec3Df::dotProduct(n,i)*Vec3Df::dotProduct(n,r);

    spec=Ks*Vec3Df::dotProduct(n,i)/sqrt(spec)*exp(-pow(tan(acos(Vec3Df::dotProduct(n,H))),2)/sig)/(4*M_PI*sig*sig);

    return {spec, spec, spec};
}

Vec3Df Brdf::wardAnisotrope(Vec3Df r, Vec3Df i, Vec3Df n) const {
    float coeff_spec;
    Vec3Df H=r+i;
    Vec3Df x,y,h;
    x=Vec3Df::crossProduct(n,i);
    y=Vec3Df::crossProduct(n,x);
    H.normalize();
    h=H-n*Vec3Df::dotProduct(n,H);
    h.normalize();

    float rugosite_x=cst[6];
    float rugosite_y=cst[7];

    coeff_spec=Ks*1/sqrt(Vec3Df::dotProduct(n,i)*Vec3Df::dotProduct(n,r))
        *1/(4*M_PI*rugosite_x*rugosite_y)
        *exp(- pow(tan(acos(Vec3Df::dotProduct(n,H))) ,2)
             *(pow(Vec3Df::dotProduct(h,x) / rugosite_x ,2) + pow(sin(acos(Vec3Df::dotProduct(h,x))) / rugosite_y ,2))
             );

    float spec = coeff_spec*Vec3Df::dotProduct(n,i);

    return {spec, spec, spec};
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
        if(type&BlinnPhong)
            currentColor += blinnPhong(ra, ir, n);
        if(type&Schlick) {
            auto coupleColor = schlick(ra, ir, n);
            if(type&SchlickDiff)
                currentColor+=coupleColor.first;
            if(type&SchlickSpec)
                currentColor+=coupleColor.second;
        }
        if(type&Ward)
            currentColor += ward(ra, ir, n);
        if(type&WardAnisotrope)
            currentColor += ward(ra, ir, n);

        color += light.getIntensity()*light.getColor()*currentColor;
    }

    if(lights.size())
        color /= lights.size();

    if(type&Ambient)
        color += ambient();

    return color;
}
