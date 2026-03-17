/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef ZGATE_H
#define ZGATE_H

#pragma once

#include "../state.h"
#include "../noisyState.h"
#include "../operation.h"

class ZGate : public Operation
{
public:
    ZGate(int idx);
    ~ZGate();

    State* ApplyBase(State* state);
    MatrixInt* ApplyToNoise(MatrixInt* noise);
    NoisyState* Apply(NoisyState* state) override;
    string ToString() override;

private:
    int mDimension;
    int mIdx;
};

#endif