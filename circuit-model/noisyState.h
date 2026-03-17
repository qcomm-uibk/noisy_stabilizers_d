/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef NOISYSTATE_H
#define NOISYSTATE_H

#pragma once

#include <vector>
#include "state.h"
#include "noisemap.h"
#include "../logging/logger.h"

using namespace std;

class NoisyState
{
public:
    NoisyState() { };
    NoisyState(State* state, vector<NoiseMap*> noiseMaps);
    ~NoisyState();

    State* getState();
    vector<NoiseMap*> getNoiseMaps();
    int getDimension();

    bool isEqual(NoisyState *other);
    void log(Logger *logger);

    void addNoiseMap(NoiseMap* noiseMap);

    NoisyState* copy() { 
        vector<NoiseMap*> copied;
        for(int i=0; i<mNoiseMaps.size(); i++)
            copied.push_back(mNoiseMaps[i]->copy());
        
        return new NoisyState(mState->copy(), copied);
    }

    NoisyState* reduceNoisyStatePostMeas(int idx);
    vector<NoiseMap*> reducedNoiseMaps(VectorInt* subset);
    NoisyState* reducedNoisyState(VectorInt* subset);
    MatrixComplex* calculateMixedStateDensityMatrix(Logger* logger);
    double fidelity(Logger* logger);

private:
    State* mState;
    vector<NoiseMap*> mNoiseMaps;
};

#endif
