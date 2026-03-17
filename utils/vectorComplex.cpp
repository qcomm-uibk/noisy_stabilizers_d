/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "vectorComplex.h"

VectorComplex::VectorComplex()
{
}

VectorComplex::VectorComplex(vector<int> vector)
{
    for(int i = 0; i < vector.size(); i++)
        mVector.push_back(vector[i]);
}

VectorComplex::~VectorComplex()
{
}