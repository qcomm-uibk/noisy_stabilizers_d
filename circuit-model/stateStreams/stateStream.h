/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef STATESTREAM_H
#define STATESTREAM_H

#include <string>
#include <vector>
#include <tuple>
#include "../noisyState.h"

#pragma once

using namespace std;

class StateStream
{
public:
    StateStream();
    ~StateStream();

    virtual NoisyState* read(string stabilizerFileName, string phaseFileName, vector<tuple<string, string>> noiseFileNames, int dimension) = 0;
    virtual void write(NoisyState *noisyState, string stabilizerFileName, string phaseFileName, vector<tuple<string, string>>  noiseFileName, int dimension) = 0;
    virtual void writeStateFile(ofstream &file, MatrixInt* matrix) = 0;
    virtual void writePhaseFile(ofstream &file, VectorInt* vector) = 0;
    virtual void writeMatrixDoubleFile(ofstream &file, MatrixDoubles* matrix) = 0;
    virtual void writeMatrixComplexFile(ofstream &file, MatrixComplex* matrix) = 0;

private:

};

#endif