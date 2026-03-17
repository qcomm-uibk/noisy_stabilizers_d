/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "phaseGate.h"
#include "../../utils/mathUtils.h"

PhaseGate::PhaseGate(int idx, vector<NoiseMap*> noiseMaps)
{
    mIdx = idx;

    for(int i=0; i<noiseMaps.size(); i++)
        mNoiseMaps.push_back(noiseMaps[i]);
}

PhaseGate::~PhaseGate()
{
    
}

NoisyState* PhaseGate::Apply(NoisyState *noisyState)
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

State* PhaseGate::ApplyBase(State *state)
{
    mDimension = state->getDimension();
    
    MatrixInt* newStabilizer = state->getStabilizer()->copy();
    VectorInt* newPhase;

    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    int n = newStabilizer->getColumnsOfRow(0) / 2;    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Phase gate mIdx out of bounds");

    newPhase = state->getPhase()->copy();
        
    int numRows = newStabilizer->getNumRows();

    //n is the number of qubits --> in the overleaf there is a bug, as numRows != n for noise maps (!!!)

    for (int i = 0; i < numRows; i++)
    {        
        auto x = state->getStabilizer()->getValue(i, mIdx+n);
        auto z = state->getStabilizer()->getValue(i,mIdx);

        // X gets mapped to XZ
        newStabilizer->addAndMod(i, mIdx, x, mDimension);

        if(mDimension == 2){
            // Y gets mapped to -X
            auto v3 = x * z;
            newPhase->addAndMod(i, v3, mDimension);
        }
    }

    return new State(newStabilizer, newPhase, mDimension);
}

MatrixInt* PhaseGate::ApplyToNoise(MatrixInt *noise)
{
    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    int n = noise->getColumnsOfRow(0) / 2;    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Phase gate mIdx out of bounds");
        
    int numRows = noise->getNumRows();
    MatrixInt* newNoise = noise->copy();

    //n is the number of qubits --> in the overleaf there is a bug, as numRows != n for noise maps (!!!)

    for (int i = 0; i < numRows; i++)
    {        
        auto x = noise->getValue(i, mIdx+n);
        auto z = noise->getValue(i,mIdx);

        // X gets mapped to XZ
        newNoise->addAndMod(i, mIdx, x, mDimension);
    }
    return newNoise;
}

string PhaseGate::ToString()
{
    return "Phase gate found for index " + to_string(mIdx);
}