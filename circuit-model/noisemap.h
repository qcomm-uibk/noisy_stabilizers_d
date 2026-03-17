/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef NOISEMAP_H
#define NOISEMAP_H

#pragma once

#include <vector>

#include "state.h"
#include "../logging/logger.h"
#include "../utils/vectorDouble.h"

using namespace std;

//NoiseMap uses internally also State, even though we only need to stabilizer tableau part of it, because
//the algorithms to update noisemaps and stabilizers look often almost identical for them --> this optimizes
//the implementation
class NoiseMap
{
public:
    NoiseMap(MatrixInt* noises, VectorDouble *weights);
    ~NoiseMap();

    MatrixInt* getNoises();
    VectorDouble* getWeights();

    bool CheckIsValid();

    void log(Logger *logger);
    bool isEqual(NoiseMap* other);

    NoiseMap* copy() { return new NoiseMap(mNoises->copy(), mWeights->copy()); }

private:
    //A noise map has two parts, the operators, and the weights. A general pauli channel can be written as a 
    //kraus decomposed map, which looks like sum_i K_i (.) K_i^*. In our representation here we use that K_i 
    //are tensors of Paulis together with a weight. Hence
    //
    // N(.) = sum_i w_i P_i (.) P_i^*
    // w_i ... weight for Pauli operator P_i
    // P_i ... tensor product of Paulis
    // i ..... runs from 0 to k-1 --> k operators
    //
    //Each row in state corresponds to a P_i in ZX representation
    //Each row in weights corresponds to the weight of Pauli P_i in the noise map
    MatrixInt* mNoises;  
    VectorDouble* mWeights;
};

#endif