/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/



#include "czGate.h"
#include "../../utils/mathUtils.h"

CzGate::CzGate(int sourceIdx, int targetIdx, vector<NoiseMap*> noiseMaps)
{
    mSourceIdx = sourceIdx;
    mTargetIdx = targetIdx;

    for(int i=0; i<noiseMaps.size(); i++)
        mNoiseMaps.push_back(noiseMaps[i]);
}

CzGate::~CzGate()
{

}

NoisyState* CzGate::Apply(NoisyState *noisyState)
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

State* CzGate::ApplyBase(State* state)
{
    mDimension = state->getDimension();

    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    MatrixInt* newStabilizer = state->getStabilizer()->copy();
    VectorInt* newPhase;

    int n = newStabilizer->getColumnsOfRow(0) / 2;    
    if(mTargetIdx >= n || mTargetIdx < 0)
        throw invalid_argument("CzGate mTargetIdx out of bounds");

    if(mSourceIdx >= n || mSourceIdx < 0)
        throw invalid_argument("CzGate mSourceIdx out of bounds");

    if(mSourceIdx == mTargetIdx)
        throw invalid_argument("CzGate mSourceIdx and mTargetIdx the same");

    
    newPhase = state->getPhase()->copy();
    int numRows = newStabilizer->getNumRows();

    //n is the number of qubits --> in the overleaf there is a bug, as numRows != n for noise maps (!!!)

    for (int i = 0; i < numRows; i++)
    {
        auto target_x = state->getStabilizer()->getValue(i, mTargetIdx + n);
        auto source_x = state->getStabilizer()->getValue(i, mSourceIdx + n);

        auto target_z = state->getStabilizer()->getValue(i, mTargetIdx);
        auto source_z = state->getStabilizer()->getValue(i, mSourceIdx);

        newStabilizer->addAndMod(i, mSourceIdx, target_x, mDimension);
        // X update rule
        newStabilizer->addAndMod(i, mTargetIdx, source_x, mDimension);

        if(mDimension == 2){
            auto v3 = source_x * target_x * (source_z + target_z);
            newPhase->addAndMod(i, v3,mDimension);
        }
    }

    return new State(newStabilizer, newPhase, state->getDimension());
}

MatrixInt* CzGate::ApplyToNoise(MatrixInt* noise)
{
    int n = noise->getColumnsOfRow(0) / 2;
    if(mTargetIdx >= n || mTargetIdx < 0)
        throw invalid_argument("CzGate mTargetIdx out of bounds");

    if(mSourceIdx >= n || mSourceIdx < 0)
        throw invalid_argument("CzGate mSourceIdx out of bounds");

    if(mSourceIdx == mTargetIdx)
        throw invalid_argument("CzGate mSourceIdx and mTargetIdx the same");

    int numRows = noise->getNumRows();
    MatrixInt* newNoise = noise->copy();

    //n is the number of qubits --> in the overleaf there is a bug, as numRows != n for noise maps (!!!)

    for (int i = 0; i < numRows; i++)
    {
        auto target_x = noise->getValue(i, mTargetIdx + n);
        auto source_x = noise->getValue(i, mSourceIdx + n);

        auto target_z = noise->getValue(i, mTargetIdx);
        auto source_z = noise->getValue(i, mSourceIdx);

        newNoise->addAndMod(i, mSourceIdx, target_x, mDimension);
        // X update rule
        newNoise->addAndMod(i, mTargetIdx, source_x, mDimension);
    }

    return newNoise;
}

string CzGate::ToString()
{
    return "CZ for control " + to_string(mSourceIdx) + " and target " + to_string(mTargetIdx);
}