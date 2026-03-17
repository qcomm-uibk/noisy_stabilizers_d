/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef PHASEGATE_H
#define PHASEGATE_H

#pragma once

#include "../state.h"
#include "../noisyState.h"
#include "../operation.h"

class PhaseGate : public Operation
{
public:
    PhaseGate(int idx, vector<NoiseMap*> noiseMaps);
    ~PhaseGate();

    State* ApplyBase(State* state);
    MatrixInt* ApplyToNoise(MatrixInt* noise);
    NoisyState* Apply(NoisyState* state) override;
    string ToString() override;

private:
    int mDimension;
    int mIdx;
    vector<NoiseMap*> mNoiseMaps;
};

#endif