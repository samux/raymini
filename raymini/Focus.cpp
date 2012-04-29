#include <cmath>
#include <iostream>

#include "Focus.h"

using namespace std;

vector<pair<float, float>> Focus::generateOffsets(Type type, float aperture, unsigned rays) {

    vector<pair<float, float>> offsets;

    switch(type) {

        case NONE:
            offsets.push_back(make_pair(0.0, 0.0));
            break;

        case UNIFORM:
            {
                int raysSqrt = ceil(sqrt(rays));
                float step = 2.0 * aperture / sqrt(2.0);
                unsigned int count = 0;
                float orig = -step*raysSqrt/2.0 + step/2.0; 
                for (unsigned int i=0; i<raysSqrt && count<rays; i++) {
                    for (int j=0; j<raysSqrt && count<rays; j++) {
                        offsets.push_back(make_pair(orig + j*step, orig + i*step));
                        count++;
                    }
                }
            }
            break;

        case STOCHASTIC:
            offsets.push_back(make_pair(0.0, 0.0));
            break;
    }

    return offsets;

}
