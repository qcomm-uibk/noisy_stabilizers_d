/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef HADAMARD_H
#define HADAMARD_H

#pragma once

#include "../state.h"
#include "../noisyState.h"
#include "../operation.h"

class Hadamard : public Operation
{
public:
    Hadamard(int idx, vector<NoiseMap*> noiseMaps);
    ~Hadamard();

    State* ApplyBase(State* state);
    MatrixInt* ApplyToNoise(MatrixInt* noise);
    NoisyState* Apply(NoisyState* noisyState) override;
    string ToString() override;

private:
    int mDimension;
    int mIdx;
    vector<NoiseMap*> mNoiseMaps;
};

#endif