/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef VECTORINT_H
#define VECTORINT_H

#pragma once

#include <vector>
#include "mathUtils.h"
#include "../logging/logger.h"

using namespace std;

class VectorInt
{
public:
    VectorInt();
    VectorInt(vector<int> vector);
    ~VectorInt();

    void initialize(int numElements, int initialValue);
    int getSize() { return mVector.size(); }
    int getValue(int i) { return mVector[i]; }
    void setValue(int i, int value) { mVector[i] = value; }
    void pushBack(int value) { mVector.push_back(value); } 
    bool isEqual(VectorInt* other);

    VectorInt* copy() { return new VectorInt(mVector); }
    vector<int> getVector() { return mVector; }
    void logVector(Logger *logger);

    VectorInt* nonzeroIndx();
    void concatenateAsSet(VectorInt* secondVector);
    bool contains(int value);
    VectorInt* sum(VectorInt* secondVector);
    void removeElement(int index) { mVector.erase(mVector.begin() + index); }
    void addAndMod(int i, int value, int d) { setValueMod(i, mVector[i]+ value, d); }
    void setValueMod(int i, int value, int d) { mVector[i] = MathUtils::mod(value, d); }
    void swapElements(int index1, int index2);
    VectorInt* getSelectedElements(VectorInt* indices);

    VectorInt* notVector();
    VectorInt* multiplyVectorInt(int value);
    int gamma();
    int beta(VectorInt* other);
    int sigma(VectorInt* w, int dimension);
    VectorInt* mod(int modValue);


private:
    vector<int> mVector;
};

#endif