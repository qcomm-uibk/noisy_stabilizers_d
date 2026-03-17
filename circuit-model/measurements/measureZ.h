/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef MEASUREZ_H
#define MEASUREZ_H

#pragma once

#include "../noisyState.h"
#include "../operation.h"

class MeasureZ : public Operation
{
public:
    MeasureZ(int idx, bool isNoisy, vector<NoiseMap*> noiseMaps, int dimension = 2);
    MeasureZ(int idx, bool isNoisy, vector<NoiseMap*> noiseMaps, int outcome, int dimension = 2);
    ~MeasureZ();

    NoisyState* Apply(NoisyState* state) override;
    string ToString() override;

private:
    int step1();
    void step2(int j);
    void step2a(int j, vector<NoiseMap*> *newNoiseMaps);
    NoiseMap* step2b(vector<NoiseMap*> *newNoiseMaps);
    void step3(int j);
    void step3b(NoiseMap* newMap, vector<NoiseMap*> *newNoiseMaps);
    
    NoiseMap* compileMaps(vector<NoiseMap*> *listOfMaps);

    int mIdx;
    bool mIsNoisy;
    NoisyState* mState;
    vector<NoiseMap*> mNoiseMaps;
    MatrixInt* mNewStabilizer;
    VectorInt* mNewPhase;
    int mNumRows;

    bool mIsOutcomeChosen;
    int mOutcome;
    int mDimension;
};

#endif