/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "multiplicationGate.h"
#include "../../utils/mathUtils.h"

MultiplicationGate::MultiplicationGate(int idx, int factor)
{
    mIdx = idx;
    mFactor = factor;
}

MultiplicationGate::~MultiplicationGate()
{

}

NoisyState* MultiplicationGate::Apply(NoisyState *noisyState)
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

State* MultiplicationGate::ApplyBase(State *state)
{
    mDimension = state->getDimension();
    MatrixInt* newStabilizer = state->getStabilizer()->copy();
    VectorInt* newPhase;

    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    int n = newStabilizer->getColumnsOfRow(0) / 2;    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Multiplication gate mIdx out of bounds");

    newPhase = state->getPhase()->copy();
    newPhase->mod(mDimension); 
    int numRows = newStabilizer->getNumRows();

    for (int i = 0; i < numRows; i++)
    {
        // a**(d-2) = a**(-1) mod d for d=prime
        newStabilizer->setValue(i, mIdx, state->getStabilizer()->getValue(i, mIdx) * MathUtils::power(mFactor, mDimension-2));
        newStabilizer->setValue(i, mIdx+n, state->getStabilizer()->getValue(i, mIdx+n) * mFactor);
    }

    newStabilizer->mod(mDimension);    

    return new State(newStabilizer, newPhase, mDimension);
}

MatrixInt* MultiplicationGate::ApplyToNoise(MatrixInt *noise)
{
    //number of qubits can ALWAYS be inferred from the number of operators in a stabilizer row (but not the nr of rows!!!)
    int n = noise->getColumnsOfRow(0) / 2;    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Multiplication gate mIdx out of bounds");

    int numRows = noise->getNumRows();
    MatrixInt* newNoise = noise->copy();

    for (int i = 0; i < numRows; i++)
    {
        // a**(d-2) = a**(-1) mod d for d=prime
        newNoise->setValue(i, mIdx, noise->getValue(i, mIdx) * MathUtils::power(mFactor, mDimension-2));
        newNoise->setValue(i, mIdx+n, noise->getValue(i, mIdx+n) * mFactor);
    }

    newNoise->mod(mDimension);  
    return newNoise;  
}

string MultiplicationGate::ToString()
{
    return "Multiplication gate for index " + to_string(mIdx) + " with factor " + to_string(mFactor);
}