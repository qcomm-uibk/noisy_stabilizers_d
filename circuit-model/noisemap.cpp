/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "noisemap.h"
#include "../utils/mathUtils.h"
#include <math.h>

NoiseMap::NoiseMap(MatrixInt* noises, VectorDouble* weights)
{
    mNoises = noises;
    mWeights = weights;
}

NoiseMap::~NoiseMap()
{
    delete mNoises;
    delete mWeights;
}

MatrixInt* NoiseMap::getNoises()
{
    return mNoises;
}

VectorDouble* NoiseMap::getWeights()
{
    return mWeights;
}

bool NoiseMap::CheckIsValid()
{
    for(int i = 0; i < mWeights->getSize(); i++)
        if (mWeights->getValue(i) < 0 || mWeights->getValue(i) > 1)
            return false;
    return true;
}

void NoiseMap::log(Logger *logger)
{
    //logger->("Noise map description: " + getDescription());
    getNoises()->logMatrix(logger);

    logger->WriteLog("Weights:");
    for(int i=0; i<mWeights->getSize(); i++)
        logger->WriteLog(to_string(mWeights->getValue(i)));
}

bool NoiseMap::isEqual(NoiseMap* other)
{
    if(mWeights->getSize() != other->mWeights->getSize())
        return false;

    //TODO rework equality for state permutations and noise maps/noisy states

    for(int i=0; i<mWeights->getSize(); i++)
    {
        //tolerance as doubles will never be exactly the same
        auto difference = fabs(mWeights->getValue(i) - other->mWeights->getValue(i));
        if(difference > MathUtils::getTolerance())
            return false;
    }

    return mNoises->isEqual(other->mNoises);
}