/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef MULTIPLICATIONGATE_H
#define MULTIPLICATIONGATE_H

#pragma once

#include "../state.h"
#include "../noisyState.h"
#include "../operation.h"

class MultiplicationGate : public Operation
{
public:
    MultiplicationGate(int idx, int factor);
    ~MultiplicationGate();

    State* ApplyBase(State* state);
    MatrixInt* ApplyToNoise(MatrixInt* noise);
    NoisyState* Apply(NoisyState* state) override;
    string ToString() override;

private:
    int mDimension;
    int mIdx;
    int mFactor;
};

#endif