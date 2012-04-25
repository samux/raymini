#include <cmath>
#include <cstdlib>

#include "AntiAliasing.h"

using namespace std;

/** Prepare a list of anti aliasing offsets for both i and j */
vector<pair<float, float> > AntiAliasing::generateOffsets(Type type, unsigned rays) {
    // Each ray picking is translated to fill the pixel which bottom left coordinate is i,j
    vector<pair<float, float>> offsets;

    switch (type) {
        case NONE:
            // One ray on the bottom left of the pixel
            offsets.push_back(make_pair(0.0, 0.0));
            break;

        case UNIFORM: {
                // Fill as well as possible the space (optimal if rays is a square)
                // Chosen algorithm:
                // - cut the pixel in ceil(sqrt(rays))^2 cells
                // - pick each center until rays is reached
                unsigned int raysSqrt = ceil(sqrt(rays));
                float cutting = 1.0/(float)(2*raysSqrt);
                unsigned int count = 0;
                for (unsigned int i=0; i<raysSqrt && count<rays; i++) {
                    for (unsigned int j=0; j<raysSqrt && count<rays; j++) {
                        offsets.push_back(make_pair(float(2*i+1)*cutting, float(2*j+1)*cutting));
                        count++;
                    }
                }
            }
            break;

        case POLYGONAL: {
                // Turn around a circle
                float angleStep = 2.0*M_PI/float(rays);
                float angle = 0.0;
                for (unsigned int i=0; i<rays; i++) {
                    float cosAngle = (cos(angle) + 1.0) / 2.0;
                    float sinAngle = (sin(angle) + 1.0) / 2.0;
                    offsets.push_back(make_pair(cosAngle, sinAngle));
                    angle += angleStep;
                }
            }
            break;

        case STOCHASTIC: {
                // Picked using randomness
                for (unsigned int i=0; i<rays; i++) {
                    float di = (float)rand() / (float)RAND_MAX;
                    float dj = (float)rand() / (float)RAND_MAX;
                    offsets.push_back(make_pair(di, dj));
                }
            }
            break;
        }

    return offsets;
}
