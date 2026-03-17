/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef CZGATE_H
#define CZGATE_H

#pragma once

#include "../state.h"
#include "../noisyState.h"
#include "../operation.h"

class CzGate : public Operation
{
public:
    CzGate(int sourceIdx, int targetIdx, vector<NoiseMap*> noiseMaps);
    ~CzGate();

    State* ApplyBase(State* state);
    MatrixInt* ApplyToNoise(MatrixInt* noise);
    NoisyState* Apply(NoisyState* state) override;
    string ToString() override;

private:
    int mDimension;
    int mSourceIdx;
    int mTargetIdx;
    vector<NoiseMap*> mNoiseMaps;
};

#endif