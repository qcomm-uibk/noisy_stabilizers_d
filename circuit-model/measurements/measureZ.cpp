/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "measureZ.h"
#include "../../utils/mathUtils.h"

MeasureZ::MeasureZ(int idx, bool isNoisy, vector<NoiseMap*> noiseMaps, int dimension):
   mIdx { idx },
   mIsNoisy { isNoisy},
   mState {}
{
    for(int i=0; i<noiseMaps.size(); i++)
        mNoiseMaps.push_back(noiseMaps[i]);

    mDimension = dimension;
}

MeasureZ::MeasureZ(int idx, bool isNoisy, vector<NoiseMap*> noiseMaps, int outcome, int dimension):
   mIdx { idx },
   mIsNoisy { isNoisy},
   mState {}
{    
    for(int i=0; i<noiseMaps.size(); i++)
        mNoiseMaps.push_back(noiseMaps[i]);

    mIsOutcomeChosen = true;
    mOutcome = outcome;
    mDimension = dimension;
}

MeasureZ::~MeasureZ()
{

}


int MeasureZ::step1()
{
    int j = -1;
    int n = mNewStabilizer->getColumnsOfRow(0) / 2;

    for (int i = 0; i < mNewStabilizer->getNumRows(); i++)
    {
        if ((mDimension == 2 && mNewStabilizer->getValue(i, mIdx+n) == 1) || 
            (mDimension != 2 && mNewStabilizer->getValue(i, mIdx+n) != 0))
        {
            j = i;
            break;
        }   
    }

    return j;
}

void MeasureZ::step2(int j)
{
    auto oldStabilizer = mState->getState()->getStabilizer();
    auto oldPhase = mState->getState()->getPhase();
    for (int a = j; a < mNumRows; a++)
    {
        if (mDimension == 2 && oldStabilizer->getValue(a, mIdx + mNumRows) == 1)
        {
            int valueBeta = oldStabilizer->getRow(a)->beta(oldStabilizer->getRow(j)) / 2;
            // element-wise addition of rows a and j
            
            //ATTENTION: here was a bug in the addition in the spec:
            //The spec said we should add new_T[j], but TEST15 was fixed by adding T[j] instead
            for (int auxCol = 0; auxCol < mNewStabilizer->getColumnsOfRow(a); auxCol++)
                mNewStabilizer->addAndMod(a,auxCol, oldStabilizer->getValue(j, auxCol), mDimension); 
            mNewPhase->setValueMod(a, oldPhase->getValue(a) + oldPhase->getValue(j) + valueBeta, mDimension);
        }
        else if (mDimension != 2 && oldStabilizer->getValue(a, mIdx + mNumRows) != 0)
        {
            // element-wise addition of rows i and j so often such that T[i,idx+n] entry vanishes
            for (int auxCol = 0; auxCol < mNewStabilizer->getColumnsOfRow(a); auxCol++)
                mNewStabilizer->addAndMod(a, auxCol, 
                    -oldStabilizer->getValue(j, auxCol) * 
                    MathUtils::power(oldStabilizer->getValue(j, mIdx+mNumRows), mDimension-2) * 
                    MathUtils::mod(oldStabilizer->getValue(a, mIdx+mNumRows), mDimension), mDimension); 
            
            VectorInt* partial = oldStabilizer->getRow(j);
            int multiplier = - MathUtils::power(oldStabilizer->getValue(j, mIdx+mNumRows), mDimension-2) * oldStabilizer->getValue(a, mIdx+mNumRows);
            partial = partial->multiplyVectorInt(multiplier);
            int valueSigma = partial->sigma(oldStabilizer->getRow(a), mDimension);
            mNewPhase->setValueMod(a, oldPhase->getValue(a) + multiplier * oldPhase->getValue(j) + valueSigma * MathUtils::power(2, mDimension-2) , mDimension);
            delete partial;
        }
    }
}

void MeasureZ::step2a(int j, vector<NoiseMap*> *newNoiseMaps)
{
    for (int index = 0; index < mState->getNoiseMaps().size(); index++)
    {
        MatrixInt* noises = mState->getNoiseMaps().at(index)->getNoises();
        MatrixInt* newNoises = noises->copy();

        int noiseRows = noises->getNumRows();
        int n = noises->getColumnsOfRow(0) / 2;

        for (int i = 0; i < noiseRows; i++)
        {
            if (mDimension == 2 && noises->getValue(i,mIdx+n) == 1)
                for (int auxCol = 0; auxCol < newNoises->getColumnsOfRow(i); auxCol++)
                    newNoises->addAndMod(i, auxCol, mState->getState()->getStabilizer()->getValue(j, auxCol), mDimension);  // phases from s can be ignored here

            else if (mDimension != 2 && noises->getValue(i, mIdx+mNumRows) != 0)
                for (int auxCol = 0; auxCol < newNoises->getColumnsOfRow(i); auxCol++)
                    newNoises->addAndMod(i, auxCol, 
                       -mState->getState()->getStabilizer()->getValue(j, auxCol) * 
                       MathUtils::power(mState->getState()->getStabilizer()->getValue(j,mIdx+mNumRows), mDimension -2) *
                       noises->getValue(i,mIdx+mNumRows), mDimension);
        }
        newNoiseMaps->push_back(new NoiseMap(newNoises, mState->getNoiseMaps().at(index)->getWeights()->copy()));
    }
}

NoiseMap* MeasureZ::step2b(vector<NoiseMap*> *newNoiseMaps)
{
    // Find all maps that do not commute with the measurement

    for (int i = 0; i < mState->getNoiseMaps().size(); i++)
        newNoiseMaps->push_back(mState->getNoiseMaps().at(i)->copy());
    
    vector<int> mapIndices;
    vector<NoiseMap*> *mapsToCompile = new vector<NoiseMap*>();

    int n = mNewStabilizer->getColumnsOfRow(0) / 2;    

    for (int i = 0; i < mState->getNoiseMaps().size(); i++)
    {
        NoiseMap* noiseMap = mState->getNoiseMaps().at(i);
        MatrixInt* N = noiseMap->getNoises();
        // If there are any nonzero entries in the (index+n)-th column
        for (int row = 0; row < N->getNumRows(); row++)
        {
            // TODO Julius: n was referred to in the beginning as the number of rows in a matrix, which works for states but NOT for noise!
            // ANSWER: number of columns/2 is n, not the number of rows ...
            if (N->getValue(row, mIdx+n) !=0)
            {
                mapIndices.push_back(i);                
                mapsToCompile->push_back(noiseMap);
                
                //AP: I don't think here should be a break according to spec, so I removed it
                //break;
            }
        }
    }
    NoiseMap* newMap = compileMaps(mapsToCompile);
    // Remove old maps
    for (int mapIndex = mapIndices.size()-1; mapIndex >= 0; mapIndex--)
        newNoiseMaps->erase(newNoiseMaps->begin() + mapIndices.at(mapIndex));
    
    delete mapsToCompile;
    return newMap;
}

void MeasureZ::step3(int j)
{
    for (int col = 0; col < mNumRows * 2; col++)
        mNewStabilizer->setValueMod(j, col, 0, mDimension);

    mNewStabilizer->setValueMod(j, mIdx, 1, mDimension);

    if(mIsOutcomeChosen)
    {
        // Chosen outcome
        mNewPhase->setValueMod(j, mOutcome, mDimension);
    }
    else
    {
        // Assumed always 0 for now, will change later.
        mNewPhase->setValueMod(j, 0, mDimension);
    }
    
}

void MeasureZ::step3b(NoiseMap* newMap, vector<NoiseMap*> *newNoiseMaps)
{
    // Find the outcome that would result from the perfect state
    State* newState = mState->getState()->XtoRrefMod();
    int resultIdx = -1;

    for (int rowIdx = 0; rowIdx < newState->getStabilizer()->getNumRows(); rowIdx++)
    {
        if (newState->getStabilizer()->getValue(rowIdx, mIdx) != 0)
        {
            resultIdx = rowIdx;
            break;
        }
    }

    if (resultIdx == -1)
        throw invalid_argument("MeasureZ::step3b: resultIdx was not fould.");
    
    int expectedOutcome = newState->getPhase()->getValue(resultIdx);
    double identityWeight;
    VectorDouble* newWeights = new VectorDouble();
    MatrixInt* newNoises = new MatrixInt();

    // "outcome" is the outcome specified by the circuit, we do not support randomly
    // picking a result for this branch at this point
    if (mOutcome == expectedOutcome)
    {
        identityWeight = 1 - newMap->getWeights()->sum();

        for (int i = 0; i < newMap->getWeights()->getSize(); i++)
        {
            if (newMap->getNoises()->getValue(i, mIdx + mNumRows) == 0)
            {
                newWeights->append(newMap->getWeights()->getValue(i));
                newNoises->addRow(newMap->getNoises()->getRow(i));
            }
        }

        newWeights = newWeights->divideByDouble(identityWeight + newWeights->sum());
    }
    else
    {
        for (int i = 0; i < newMap->getWeights()->getSize(); i++)
        {
            if (newMap->getNoises()->getValue(i, mIdx + mNumRows) == 1)
            {
                newWeights->append(newMap->getWeights()->getValue(i));
                newNoises->addRow(newMap->getNoises()->getRow(i));
            }
        }
    
        newWeights = newWeights->divideByDouble(newWeights->sum());
    }

    newNoiseMaps->push_back(new NoiseMap(newNoises, newWeights));
    
    mNewStabilizer = newState->getStabilizer();
    mNewPhase = newState->getPhase();
}

NoiseMap* MeasureZ::compileMaps(vector<NoiseMap*> *listOfMaps)
{
    int numMaps = listOfMaps->size();

    if (numMaps == 0)
        throw new invalid_argument("MeasureZ::composeMaps: the list of maps is empty");
    else if (numMaps == 1)
        return listOfMaps->at(0);

    NoiseMap* currentMap = listOfMaps->at(0);
    for (int i = 1; i < numMaps; i++)
    {
        VectorDouble* newWeights = new VectorDouble();
        MatrixInt* newNoises = new MatrixInt();

        NoiseMap* nextMap = listOfMaps->at(i);

        double identityWeightCurrent = 1.0 - currentMap->getWeights()->sum();
        double identityWeightNext = 1.0 - nextMap->getWeights()->sum();

        for (int j = 0; j < currentMap->getWeights()->getSize(); j++){
            newWeights->append(identityWeightNext * currentMap->getWeights()->getValue(j));
            newNoises->addRow(currentMap->getNoises()->getRow(j));
        }

        for (int j = 0; j < nextMap->getWeights()->getSize(); j++){
            newWeights->append(identityWeightCurrent * nextMap->getWeights()->getValue(j));
            newNoises->addRow(nextMap->getNoises()->getRow(j));
        }

        for (int j = 0; j < currentMap->getWeights()->getSize(); j++)
        {
            for (int k = 0; k < nextMap->getWeights()->getSize(); k++)
            {
                newWeights->append(currentMap->getWeights()->getValue(j) * nextMap->getWeights()->getValue(k));
                newNoises->addRow(currentMap->getNoises()->getRow(j)->sum(nextMap->getNoises()->getRow(k)));
            }
        }

        newNoises->mod(mDimension);
        currentMap = new NoiseMap(newNoises, newWeights);
    }
    return currentMap;
}

NoisyState* MeasureZ::Apply(NoisyState *noisyState)
{
    //assign noisy input state to mState, otherwise its empty
    mState = noisyState;

    //noise maps of a gate act before the gate itself
    for(int i=0; i < mNoiseMaps.size(); i++)
        mState->addNoiseMap(mNoiseMaps[i]);

    //we have a noisy gate if we have more than one noise map
    mIsNoisy = mState->getNoiseMaps().size()>0;

    vector<NoiseMap*> newNoiseMaps;

    mNewStabilizer = mState->getState()->getStabilizer()->copy();
    mNewPhase = mState->getState()->getPhase()->copy();

    //mNumRows corresponds to n, the number of systems, because its a copy of the stabilizer tableau
    mNumRows = mNewStabilizer->getNumRows();

    int n = mNewStabilizer->getColumnsOfRow(0) / 2;
    
    if(mIdx >= n || mIdx < 0)
        throw invalid_argument("Hadamard mIdx out of bounds");

    // step 1
    int j = step1();

    if (j != -1)
    {
        // step 2
        step2(j);

        if (mIsNoisy)
            step2a(j, &newNoiseMaps);
        
        // step 3
        step3(j);
    }
    else
    {
        NoiseMap* newMap = step2b(&newNoiseMaps);
        step3b(newMap, &newNoiseMaps);

        for(int i=0; i<mNewStabilizer->getNumRows(); i++){
            if(mNewStabilizer->getRow(i)->getValue(mIdx) == 1){
                mNewPhase->setValue(i, mOutcome);
            }
        }

        delete newMap;
    }

    State* newState = new State(mNewStabilizer, mNewPhase, mState->getState()->getDimension());
    return new NoisyState(newState, newNoiseMaps);
}

string MeasureZ::ToString()
{
    if(mIsOutcomeChosen)
        return "Z measurement for index " + to_string(mIdx) + " with expected outcome " + to_string(mOutcome);
    return "Z measurement for index " + to_string(mIdx);
}