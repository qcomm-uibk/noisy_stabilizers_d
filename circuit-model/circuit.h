/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>

#include "noisyState.h"
#include "operation.h"
#include "../logging/logger.h"

#pragma once

using namespace std;
class Circuit
{
public:
    Circuit(int dimension, int numberOfQubits, Logger *logger);
    ~Circuit();

    void AddOperation(Operation* op);
    vector<Operation*>* GetOperations();

    NoisyState* Apply(NoisyState* input);

    int getDimension();
    int getNumberOfQubits();

private:
    vector<Operation*> *mOperations;
    int mDimension;
    int mNumberOfQubits;
    Logger *mLogger;
};

#endif