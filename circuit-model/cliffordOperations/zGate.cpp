/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "zGate.h"
#include "../../utils/mathUtils.h"

ZGate::ZGate(int idx)
{
    mIdx = idx;
}

ZGate::~ZGate()
{

}

NoisyState* ZGate::Apply(NoisyState *noisyState)
{
    mDimension = noisyState->getDimension();
    vector<NoiseMap*> noiseMaps = noisyState->getNoiseMaps();

    State* newPerfectState = ApplyBase(noisyState->getState());
    vector<NoiseMap*> newNoiseMaps;

    for (int i = 0; i < noiseMaps.size(); i++)
    {
        NoiseMap* newMap = new NoiseMap(ApplyToNoise(noiseMaps.at(i)->getNoises()), noiseMaps.at(i)->getWeights()->copy());
        newNoiseMaps.push_back(newMap);
    }

    return new NoisyState(newPerfectState, newNoiseMaps);
}

State* ZGate::ApplyBase(State* state)
{
    mDimension = state->getDimension();
    MatrixInt* newStabilizer = state->getStabilizer()->copy();
    VectorInt* newPhase;

    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    int n = newStabilizer->getColumnsOfRow(0) / 2;    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Z gate mIdx out of bounds");

    newPhase = state->getPhase()->copy();
    int numRows = newStabilizer->getNumRows();

    for (int i = 0; i < numRows; i++)
    {
        int temp = -state->getStabilizer()->getValue(i, mIdx+n);
        newPhase->addAndMod(i, temp, mDimension);
    }
    
    newStabilizer->mod(mDimension);

    return new State(newStabilizer, newPhase, mDimension);
}

MatrixInt* ZGate::ApplyToNoise(MatrixInt* noise)
{
    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    int n = noise->getColumnsOfRow(0) / 2;    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Z gate mIdx out of bounds");
    
    MatrixInt* newNoise = noise->copy();
    newNoise->mod(mDimension);

    return newNoise;
}

string ZGate::ToString()
{
    return "Z gate found for index " + to_string(mIdx);
}