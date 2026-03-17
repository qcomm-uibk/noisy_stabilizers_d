/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#include "hadamard.h"
#include "../../utils/mathUtils.h"

Hadamard::Hadamard(int idx, vector<NoiseMap*> noiseMaps)
{
    mIdx = idx;

    for(int i=0; i<noiseMaps.size(); i++)
        mNoiseMaps.push_back(noiseMaps[i]);
}

Hadamard::~Hadamard()
{

}

NoisyState* Hadamard::Apply(NoisyState *noisyState)
{
    mDimension = noisyState->getDimension();
    vector<NoiseMap*> noiseMaps = noisyState->getNoiseMaps();

    //noise maps of a gate act before the gate itself
    for(int i=0; i < mNoiseMaps.size(); i++)
        noiseMaps.push_back(mNoiseMaps[i]);

    State* newPerfectState = ApplyBase(noisyState->getState());
    vector<NoiseMap*> newNoiseMaps;

    for (int i = 0; i < noiseMaps.size(); i++)
    {
        NoiseMap* newMap = new NoiseMap(ApplyToNoise(noiseMaps.at(i)->getNoises()), noiseMaps.at(i)->getWeights()->copy());
        newNoiseMaps.push_back(newMap);
    }

    return new NoisyState(newPerfectState, newNoiseMaps);
}

State* Hadamard::ApplyBase(State *state)
{
    mDimension = state->getDimension();

    //A noise map can have an arbitrary number of operators. For example, a noise map for bitflip has only two rows
    //even though the map acts on n qubits -->
    // 0 0 0 ... 0 0 0 0 0
    // 0 0 0 ... 0 1 0 0 0 
    // --> only two rows, but acts on n qubits --> hence, the numRows does NOT correspond to the number of qubits, but
    // number of columns divided by two does (!!!)       

    MatrixInt* newStabilizer = state->getStabilizer()->copy();
    VectorInt* newPhase;

    int n = newStabilizer->getColumnsOfRow(0) / 2;
    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Hadamard mIdx out of bounds");

    newPhase = state->getPhase()->copy();
    int numRows = newStabilizer->getNumRows();

    for (int i = 0; i < numRows; i++)
    {
        auto x = state->getStabilizer()->getValue(i,mIdx+n);
        auto z = -state->getStabilizer()->getValue(i,mIdx);

        newStabilizer->setValueMod(i, mIdx, x, mDimension);
        newStabilizer->setValueMod(i, mIdx+n, z, mDimension);
        
        if(mDimension == 2){ 
            // Y gets mapped to -Y
            auto v3 = z * x;
            newPhase->addAndMod(i, v3, mDimension);
        }
    }

    return new State(newStabilizer, newPhase, state->getDimension());
}


MatrixInt* Hadamard::ApplyToNoise(MatrixInt *noise)
{
    //A noise map can have an arbitrary number of operators. For example, a noise map for bitflip has only two rows
    //even though the map acts on n qubits -->
    // 0 0 0 ... 0 0 0 0 0
    // 0 0 0 ... 0 1 0 0 0 
    // --> only two rows, but acts on n qubits --> hence, the numRows does NOT correspond to the number of qubits, but
    // number of columns divided by two does (!!!)       

    int n = noise->getColumnsOfRow(0) / 2;
    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Hadamard mIdx out of bounds");

    int numRows = noise->getNumRows();
    MatrixInt* newNoise = noise->copy();

    for (int i = 0; i < numRows; i++)
    {
        auto x = noise->getValue(i,mIdx+n);
        auto z = -noise->getValue(i,mIdx);

        newNoise->setValueMod(i, mIdx, x, mDimension);
        newNoise->setValueMod(i, mIdx+n, z, mDimension);
    }

    return newNoise;
}


string Hadamard::ToString()
{
    return "Hadamard for index " + to_string(mIdx);
}