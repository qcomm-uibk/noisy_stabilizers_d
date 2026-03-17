/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef CSVSTATESTREAM_H
#define CSVSTATESTREAM_H

#include <string>
#include <fstream>

#include "../noisyState.h"
#include "../../logging/logger.h"
#include "stateStream.h"

#pragma once

using namespace std;

class CsvStateStream : public StateStream
{
public:
    CsvStateStream(Logger *logger);
    ~CsvStateStream();

    NoisyState* read(string stabilizerFileName, string phaseFileName, vector<tuple<string, string>>  noiseFileNames, int dimension) override;
    void write(NoisyState *noisyState, string stabilizerFileName, string phaseFileName, vector<tuple<string, string>> noiseFileName, int dimension) override;
    void writeStateFile(ofstream &file, MatrixInt* matrix) override;
    void writePhaseFile(ofstream &file, VectorInt* vector) override;
    void writeMatrixDoubleFile(ofstream &file, MatrixDoubles* matrix) override;
    void writeMatrixComplexFile(ofstream &file, MatrixComplex* matrix) override;
    MatrixComplex* parseMatrixComplexFile(ifstream &file);
    
private:
    Logger *mLogger;

    MatrixInt* parseStateFile(ifstream &file);
    VectorInt* parsePhaseFile(ifstream &file);
    VectorDouble* parseWeightsFile(ifstream &file);

    vector<int> split(string s, string delimiter);

    void writeWeightsFile(ofstream &file, VectorDouble* weights);
};

#endif