/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "csvStateStream.h"
#include <iostream>
#include <tuple>
#include <vector>

#include "../noisemap.h"
#include "../../utils/matrix.h"
#include "../../utils/parseUtils.h"

using namespace std;

CsvStateStream::CsvStateStream(Logger *logger)
{
    this->mLogger = logger;
}

CsvStateStream::~CsvStateStream()
{
}

NoisyState* CsvStateStream::read(string stabilizerFileName, string phaseFileName, vector<tuple<string, string>> noiseFileNames, int dimension)
{
    ifstream stabilizerFs(stabilizerFileName);
    ifstream phaseFs(phaseFileName);
    
    auto stabilizer = parseStateFile(stabilizerFs);
    auto phase = parsePhaseFile(phaseFs);

    vector<NoiseMap*> noiseMaps;

    int numNoiseMaps = noiseFileNames.size();
    for (int i = 0; i < numNoiseMaps; i++)
    {
        string stateName = std::get<0>(noiseFileNames[i]);
        string weightsName =  std::get<1>(noiseFileNames[i]);

        ifstream stateFile(stateName);
        ifstream weightsFile(weightsName);

        auto noise = parseStateFile(stateFile);
        auto weights = parseWeightsFile(weightsFile);

        NoiseMap* n = new NoiseMap(noise, weights);
        noiseMaps.push_back(n);
    }

    State* readState = new State(stabilizer,phase,dimension);

    if(mLogger)
        readState->Log(mLogger);

    return new NoisyState(readState, noiseMaps);
}
    
void CsvStateStream::write(NoisyState* noisyState, string stabilizerFileName, string phaseFileName, vector<tuple<string, string>>  noiseFileNames, int dimension)
{
    ofstream stabilizerFs(stabilizerFileName);
    ofstream phaseFs(phaseFileName);
    
    writeStateFile(stabilizerFs, noisyState->getState()->getStabilizer());   
    writePhaseFile(phaseFs, noisyState->getState()->getPhase());    

    vector<NoiseMap*> noiseMaps = noisyState->getNoiseMaps();
    int numNoiseMaps = noiseMaps.size();

    for(int i=0; i<numNoiseMaps; i++)
    {
        NoiseMap* n = noiseMaps[i];

        string stateName = std::get<0>(noiseFileNames[i]);
        string weightsName =  std::get<1>(noiseFileNames[i]);

        ofstream stateFile(stateName);
        ofstream weightsFile(weightsName);

        writeStateFile(stateFile, n->getNoises());
        writeWeightsFile(weightsFile, n->getWeights());
    }
}

void CsvStateStream::writeStateFile(ofstream &file, MatrixInt* matrix)
{
    string delim(",");
    for (int i = 0; i < matrix->getNumRows(); i++)
    {
        for (int j = 0; j < matrix->getColumnsOfRow(i); j++)
        {
            file << to_string(matrix->getValue(i,j));
            if(j != matrix->getColumnsOfRow(i) - 1)
                file << delim; 
        }
        if(i != matrix->getNumRows() - 1)
            file << std::endl;
    }
}

void CsvStateStream::writePhaseFile(ofstream &file, VectorInt* vector)
{
    for (int i = 0; i < vector->getSize(); i++)
    {
        file << to_string(vector->getValue(i));
        if(i != vector->getSize() - 1)
            file << std::endl;
    }
}

void CsvStateStream::writeMatrixDoubleFile(ofstream &file, MatrixDoubles* matrix)
{
    string delim(",");
    for (int i = 0; i < matrix->getNumRows(); i++)
    {
        for (int j = 0; j < matrix->getColumnsOfRow(i); j++)
        {
            file << to_string(matrix->getValue(i,j));
            if(j != matrix->getColumnsOfRow(i) - 1)
                file << delim; 
        }
        if(i != matrix->getNumRows() - 1)
            file << std::endl;
    }
}

void CsvStateStream::writeMatrixComplexFile(ofstream &file, MatrixComplex* matrix)
{
    string delim(",");
    for (int i = 0; i < matrix->getNumRows(); i++)
    {
        for (int j = 0; j < matrix->getColumnsOfRow(i); j++)
        {
            double real = matrix->getValue(i,j).real();
            double imag = matrix->getValue(i,j).imag();

            string number("");
            if(imag == 0.0)
                number = to_string(real);
            else {
                if(imag < 0)
                    number = to_string(real) + to_string(imag) + "j"; 
                else
                    number = to_string(real) + "+" + to_string(imag) + "j"; 
            }
            file << number;
            if(j != matrix->getColumnsOfRow(i) - 1)
                file << delim; 
        }
        if(i != matrix->getNumRows() - 1)
            file << std::endl;
    }
}

void CsvStateStream::writeWeightsFile(ofstream &file, VectorDouble* weights)
{
    for (int i = 0; i < weights->getSize(); i++)
    {
        file << to_string(weights->getValue(i));

        if(i != weights->getSize() - 1)
            file << std::endl;
    }
}

MatrixComplex* CsvStateStream::parseMatrixComplexFile(ifstream &file)
{
    string delim(",");
    vector<vector<complex<double>>> state;
    for( std::string line; getline(file,line); )
    {
        vector<complex<double>> row=ParseUtils::splitComplex(line, delim);
        state.push_back(row);
    }

    return new MatrixComplex(state);
}


MatrixInt* CsvStateStream::parseStateFile(ifstream &file)
{
    string delim(",");
    vector<vector<int>> state;
    for( std::string line; getline(file,line); )
    {
        vector<int> row=ParseUtils::split(line, delim);
        state.push_back(row);
    }

    return new MatrixInt(state);
}

VectorInt* CsvStateStream::parsePhaseFile(ifstream &file)
{
    VectorInt* result = new VectorInt();
    for( std::string line; getline(file,line); )
    {        
        int res = stoi(line);
        if(res < 0)
            throw invalid_argument("integer in phase file negative!");

        result->pushBack(res);
    }
    return result;
}

VectorDouble* CsvStateStream::parseWeightsFile(ifstream &file)
{
    vector<double> weights;
    for( std::string line; getline(file,line); )
    {
        double weight=stof(line);
        weights.push_back(weight);
    }

    return new VectorDouble(weights);
}