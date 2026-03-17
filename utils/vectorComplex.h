/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef VECTORCOMPLEX_H
#define VECTORCOMPLEX_H

#pragma once

#include <vector>
#include <complex>
#include "../logging/logger.h"

using namespace std;

class VectorComplex
{
public:
    VectorComplex();
    VectorComplex(vector<int> vector);
    ~VectorComplex();

    int getSize() { return mVector.size(); }
    complex<double> getValue(int i) { return mVector[i]; }
    void setValue(int i, complex<double> value) {mVector[i] = value; }
    void pushBack(complex<double> value) { mVector.push_back(value); }
    vector<complex<double>> getVector() { return mVector; }

private:
    // defines the complex number: (10 + 2i)
    vector<complex<double>> mVector;

};

#endif