/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "quantumChannel.h"

QuantumChannel::QuantumChannel(NoiseMap *noiseMap, string description) :
    mNoiseMap(noiseMap),
    mDescription(description)
{
}

QuantumChannel::~QuantumChannel()
{

}

NoisyState* QuantumChannel::Apply(NoisyState *noisyState)
{
    State* perfectState = noisyState->getState()->copy();

    vector<NoiseMap*> noiseMaps = noisyState->getNoiseMaps();
    vector<NoiseMap*> newNoiseMaps;

    for(int i=0; i<noiseMaps.size(); i++)
        newNoiseMaps.push_back(noiseMaps[i]->copy());

    //channels means just pushing back the noise map
    newNoiseMaps.push_back(mNoiseMap->copy());

    return new NoisyState(perfectState, newNoiseMaps);
}

string QuantumChannel::ToString()
{
    return mDescription;
}