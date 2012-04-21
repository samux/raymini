#pragma once

#include <vector>

#include "Vec3D.h"

/*
TODO:
Smooth for perlin 3/4D?
*/

class LCG {
    protected:
        unsigned int lcgCurrent;

    public:
        static const unsigned int MAX_RAND = (((1<<30) -1)<<1)+1; // 2^31-1

        LCG(unsigned int seed): lcgCurrent(seed) {}

        unsigned int rand() {
            lcgCurrent= lcgCurrent*1103515245u+12345u;
            return lcgCurrent&(MAX_RAND);
        }
};

class Noise: public LCG {
    private:
        static const unsigned int gaussianNoiseIterations = 100;

    public:
        Noise();

        // virtual float operator()(float x, float y) =0;
        // virtual float operator()(const Vec3Df &p) =0;
        // virtual float operator()(const Vec3Df &p, float time) =0;
        // virtual float operator()(const Vec3Df &p, const Vec3Df &n) =0;

        float uniform(); //[0,1]
        float uniform(float a, float b);
        float gaussianNoise(); //mean = 0, var = 1
        float gaussianNoise(float var, float mean=0);
        unsigned int poisson(float mean);

        static float cosineInterpolation(float a, float b, float x);

    protected:
        static float noiseTo01(float n) {
            if(n<=-1.f) return 0.f;
            if(n>=1.f) return 1.f;
            return (n+1)/2;
        }
};

/******************************************************************************/

class Wavelet: public Noise {
    private:
        float *noiseTileData;
        int noiseTileSize;
        float gaussianVar;

    public:
        float s;
        int firstBand;
        std::vector<float> w;

        Wavelet(int n, float s, int firstBand, float gaussianVar):
            gaussianVar(gaussianVar), s(s), firstBand(firstBand){
                generateNoiseTile(n);
                setW([](unsigned i) ->float{ return 1.0f/pow(2,i); }, 5); //octave
            }

        ~Wavelet() {
            delete[] noiseTileData;
        }

        void generateNoiseTile(int n, float variance=0.f) {
            if(variance > 0.f) gaussianVar = variance;
            if (n%2) n++; // a tile size must be even
            if(n > 0) noiseTileSize = n;
            generateNoiseTile();
        }
        void generateNoiseTile();

        void setW(float (*f) (unsigned int), unsigned int nbands) {
            if(nbands<=0)
                return;

            w.resize(nbands);
            for(unsigned int i = 0 ; i < nbands ; i++)
                w[i] = f(i);
        }

        float operator()(const Vec3Df &p) {
            return multibandNoise(p, nullptr);
        }

        float operator()(const Vec3Df &p, const Vec3Df &normal) {
            return multibandNoise(p, &normal);
        }

        int getNoiseTileSize() const { return noiseTileSize; }
        float getNoiseVar() const { return gaussianVar; }

    private:
        float random() {
            float noise = Noise::gaussianNoise(gaussianVar);
            return (noise<-1.f)?-1.f:((noise>1.f)?1.f:noise);
        }

        static int mod(int x, int n) {
            int m=x%n;
            return (m<0) ? m+n : m;
        }

        static void downsample (float *from, float *to, int n, int stride);
        static void upsample( float *from, float *to, int n, int stride);

        float wNoise(const Vec3Df &p);
        float wProjectedNoise(const Vec3Df &p, const Vec3Df &normal);
        float multibandNoise(const Vec3Df &p, const Vec3Df *normal);
};

/******************************************************************************/

class Perlin: Noise {
    private:
        enum class Dimension { D2, D3, D4 };

    public:
        float p;
        int n;
        float f0;

        Perlin(float persistence, float nbIterations, float f0):
            p(persistence), n(nbIterations), f0(f0) {}

        float operator()(float x, float y) const{
            return compute(Dimension::D2, x, y);
        }
        float operator()(const Vec3Df &p) const{
            return compute(Dimension::D3, p[0], p[1], p[2]);
        }
        float operator()(const Vec3Df &p, float time) const{
            return compute(Dimension::D4, p[0], p[1], p[2], time);
        }

    private:
        static inline float interpolate(float a, float b, float x) {
            return cosineInterpolation(a, b, x);
        }

        static inline float noise(int x, int y=0, int z=0, int t=0);
        static float smoothNoise(float x, float y);
        static float interpolatedNoise_smooth(float x, float y);
        static float interpolatedNoise(float x, float y, int z=0, int t=0);
        static float interpolatedNoise(float x, float y, float z, int t=0);
        static float interpolatedNoise(float x, float y, float z, float t);

        float compute(Dimension dim, float x, float y, float z=0.f, float t=0.f) const;
};

/******************************************************************************/

class Gabor : Noise{
    private:
        float K;
        float a;
        float f0;
        float nbImpulsesPerKernel;

        float kernelRadius;
        float impulseDensity;
        float var;

    public:
        unsigned randomOffset;
        float omega0;
        float varCoeff;
        bool isotropic;

        Gabor(float K, float a, float f0, float omega0,
                float number_of_impulses_per_kernel,
                unsigned randomOffset, float varCoeff, bool isotropic);

        float operator()(float x, float y);

        void setK(float K) {
            this->K = K;
            var = variance();
        }

        void setA(float a) {
            this->a = a;
            computeKRadius();
        }

        void setF0(float f0) {
            this->f0 = f0;
            var= variance();
        }

        void setNbImpulsesPerKernel(float nb) {
            nbImpulsesPerKernel = nb;
            computeIDensity();
        }

        float getK() const { return K; }
        float getA() const { return a; }
        float getF0() const { return f0; }
        float getNbImpulses() const { return nbImpulsesPerKernel; }

    private:
        static float gabor(float K, float a, float f0, float omega0, float x, float y);
        static unsigned int morton(unsigned x, unsigned y);
        float cell(int i, int j, float x, float y);

        void computeKRadius() {
            kernelRadius=sqrt(-std::log(0.05) / M_PI) / a;
            computeIDensity();
        }

        void computeIDensity() {
            impulseDensity = nbImpulsesPerKernel/(M_PI*kernelRadius*kernelRadius);
            var = variance();
        }
        float variance() const;
};
