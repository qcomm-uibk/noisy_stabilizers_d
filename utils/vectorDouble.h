/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef VECTOR_H
#define VECTOR_H

#pragma once

#include <vector>
using namespace std;

class VectorDouble
{
public:
    VectorDouble() { }
    VectorDouble(vector<double> vector);
    ~VectorDouble();

    int getSize() { return mVector.size(); }
    double getValue(int i) { return mVector[i]; }
    void setValue(int i, double value) { mVector[i] = value; }
    void append(double value) { mVector.push_back(value); }
    VectorDouble* copy() { return new VectorDouble(mVector); }
    double sum();
    VectorDouble* divideByDouble(double doubleValue);
    vector<double> getVector() { return mVector; }

private:
    vector<double> mVector;
};

#endif