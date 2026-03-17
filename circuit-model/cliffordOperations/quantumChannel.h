/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef QUANTUMCHANNEL_H
#define QUANTUMCHANNEL_H

#pragma once

#include <string>

#include "../noisyState.h"
#include "../operation.h"
#include "../noisemap.h"

class QuantumChannel : public Operation
{
public:
    QuantumChannel(NoiseMap *noiseMap, string description);
    ~QuantumChannel();

    NoisyState* Apply(NoisyState* state) override;
    string ToString() override;

private:
    NoiseMap *mNoiseMap;
    string mDescription;
};

#endif