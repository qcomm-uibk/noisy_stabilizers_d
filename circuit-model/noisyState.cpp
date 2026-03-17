/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "noisyState.h"

NoisyState::NoisyState(State* state, vector<NoiseMap*> noiseMaps)   
{
    mState = state;

    for(int i=0; i<noiseMaps.size(); i++)
        mNoiseMaps.push_back(noiseMaps[i]);
}

NoisyState::~NoisyState()
{
    delete mState;

    for(int i=0; i<mNoiseMaps.size(); i++)
        delete mNoiseMaps[i];
}

State* NoisyState::getState()
{
    return mState;
}

vector<NoiseMap*> NoisyState::getNoiseMaps()
{
    return mNoiseMaps;
}

int NoisyState::getDimension()
{
    return mState->getDimension();
}

bool NoisyState::isEqual(NoisyState *other)
{
    bool isStateEqual = mState->isEqual(other->mState);
    if(!isStateEqual)
        return false;
    
    if(other->mNoiseMaps.size() != mNoiseMaps.size())
        return false;

    //TODO rework equality for state permutations and noise maps/noisy states
    
    for(int i=0; i<mNoiseMaps.size(); i++)
    {
        NoiseMap* n = mNoiseMaps[i];
        NoiseMap* nOther = other->mNoiseMaps[i];
        
        if(!n->isEqual(nOther))
            return false;
    }

    return true;
}

void NoisyState::log(Logger *logger)
{
    logger->WriteLog("************************");
    logger->WriteLog("Noisy state description:");
    mState->Log(logger);
    logger->WriteLog("Noise maps:");
    for(int i=0; i<mNoiseMaps.size(); i++)
    {
        NoiseMap* n = mNoiseMaps[i];
        logger->WriteLog("Noisemap["+to_string(i)+"]");
        n->log(logger);
        
    }
    logger->WriteLog("************************");
}

void NoisyState::addNoiseMap(NoiseMap* noiseMap)
{
    mNoiseMaps.push_back(noiseMap);
}

NoisyState* NoisyState::reduceNoisyStatePostMeas(int idx)
{
    vector<NoiseMap*> newMaps;
    int n = mState->getStabilizer()->getColumnsOfRow(0) / 2;

    for (int i = 0; i < mNoiseMaps.size(); i++)
    {
        MatrixInt* newNoises = mNoiseMaps.at(i)->copy()->getNoises();

        newNoises->removeColumn(idx);
        newNoises->removeColumn(idx+n-1);
        newMaps.push_back(new NoiseMap(newNoises, mNoiseMaps.at(i)->getWeights()));
    }
    State* newPerfectState = mState->reduceTableauPostMeas(idx);
    return new NoisyState(newPerfectState, newMaps);
}

MatrixComplex* NoisyState::calculateMixedStateDensityMatrix(Logger* logger)
{
    // Step 1: Initialize maps and state
    MatrixInt* Z = new MatrixInt();
    MatrixInt* X = new MatrixInt();
    MatrixInt* id = new MatrixInt();
    getState()->createPauliMatrices(Z, X, id);

    MatrixComplex* pureStateDensityMatrix = getState()->pureStateDensityMatrix();
    int n = mState->getStabilizer()->getColumnsOfRow(0) / 2;    
    
    MatrixInt* idN = id->copy();
    for (int i = 0; i < n-1; i++)
        idN = getState()->kroneckerProduct(idN, id);
    vector<NoiseMap*> noise_map = getNoiseMaps();

    // Step 2
    MatrixComplex* mixedStateDensityMatrix = pureStateDensityMatrix->copy();

    for (int noiseMapIndex = 0; noiseMapIndex < noise_map.size(); noiseMapIndex++)
    {
        int k = noise_map[noiseMapIndex]->getNoises()->getNumRows();

        double identityWeight = 1.0 - noise_map[noiseMapIndex]->getWeights()->sum();
        if(identityWeight < 0.0)
            identityWeight = 0.0;

        MatrixComplex* mixedStateIntermediate = mixedStateDensityMatrix->multiplyByDouble(identityWeight, true);
        for (int i = 0; i < k; i++)
        { 
            MatrixInt* gInt = new MatrixInt();
            MatrixInt* newZ = Z->matrixExp(noise_map[noiseMapIndex]->getNoises()->getValue(i,0));
            MatrixInt* newX = X->matrixExp(noise_map[noiseMapIndex]->getNoises()->getValue(i,n));
            gInt = newZ->multiplyMatrices(newX);

            for (int j = 1; j < n; j++)
            { 
                newZ = Z->matrixExp(noise_map[noiseMapIndex]->getNoises()->getValue(i, j));
                newX = X->matrixExp(noise_map[noiseMapIndex]->getNoises()->getValue(i, j+n));

                newZ = newZ->multiplyMatrices(newX);
                gInt = getState()->kroneckerProduct(gInt, newZ);
            }

            MatrixComplex* gIntComplex = new MatrixComplex(gInt);

            MatrixComplex* partialValue = gIntComplex->complexConjugate(true);
            partialValue = partialValue->transpose(false);
            partialValue = mixedStateDensityMatrix->multiplyMatrices(partialValue);
            partialValue = gIntComplex->multiplyMatrices(partialValue);
            partialValue = partialValue->multiplyByDouble(noise_map[noiseMapIndex]->getWeights()->getValue(i), false);
            mixedStateIntermediate = mixedStateIntermediate->sum(partialValue, false);

            delete gInt;
            delete newZ;
            delete newX;
            delete gIntComplex;
            delete partialValue;
        }
        mixedStateDensityMatrix = mixedStateIntermediate;
    }

    delete Z;
    delete X;
    delete id;

    return mixedStateDensityMatrix;
}

double NoisyState::fidelity(Logger* logger)
{
    MatrixComplex* mixedStateDensityMatrix = calculateMixedStateDensityMatrix(logger);
    MatrixComplex* pureStateDensityMatrix = getState()->pureStateDensityMatrix();
    MatrixComplex* rootPure = pureStateDensityMatrix->sqrt(true);

    MatrixComplex* partialValue = rootPure->multiplyMatrices(mixedStateDensityMatrix);
    partialValue = partialValue->multiplyMatrices(rootPure);
    partialValue = partialValue->sqrt(false);
    complex<double> result = partialValue->trace();

    delete mixedStateDensityMatrix;
    delete pureStateDensityMatrix;
    delete rootPure;
    delete partialValue;

    if (result.imag() != 0)
        throw new invalid_argument("NoisyState::fidelity: Trace result imaginary part can't be different to 0");
    if (result.real() < 0 || result.real() >1)
        throw new invalid_argument("NoisyState::fidelity: Trace result real part can't smaller than 0 or bigger than 1.");

    return result.real();
}

vector<NoiseMap*> NoisyState::reducedNoiseMaps(VectorInt* subset)
{
    // subset is a list of indices
    vector<NoiseMap*> newMaps;
    for (int i = 0; i < getNoiseMaps().size(); i++)
    {
        int numRows = getNoiseMaps().at(i)->getNoises()->getNumRows();
        int numCols = getNoiseMaps().at(i)->getNoises()->getColumnsOfRow(0);
        int colOffset = numCols/2;      // Integer division
        
        VectorInt* rowsToSelect = new VectorInt();
        for (int j = 0; j < numRows; j++)
            rowsToSelect->pushBack(j);
        
        VectorInt* colsToSelect = subset->copy();
        for (int index = 0; index < subset->getSize(); index++)
            colsToSelect->pushBack(subset->getValue(index)+colOffset);

        newMaps.push_back(new NoiseMap(getNoiseMaps().at(i)->getNoises()->getSelectedElements(rowsToSelect, colsToSelect), 
                                        getNoiseMaps().at(i)->getWeights()));
    }

    return newMaps;
}

NoisyState* NoisyState::reducedNoisyState(VectorInt* subset)
{
    State* newState = getState()->reduceSeparableTableau(subset);

    // If the above gives no error, we are allowed to proceed
    vector<NoiseMap*> newNoiseMaps = reducedNoiseMaps(subset);
    return new NoisyState(newState, newNoiseMaps);
}
