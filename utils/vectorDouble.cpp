/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "vectorDouble.h"
#include <stdexcept>

VectorDouble::VectorDouble(vector<double> vector):
    mVector{vector}
{
}

VectorDouble::~VectorDouble()
{
}

double VectorDouble::sum()
{
    double result = 0;
    for (int i = 0; i < mVector.size(); i++)
        result += mVector.at(i);
    return result;
}

VectorDouble* VectorDouble::divideByDouble(double doubleValue)
{
    if (doubleValue == 0)
        throw invalid_argument("VectorDouble::divideByDouble: Divide by 0 is not supported.");

    VectorDouble* result = copy();

    for(int i = 0; i < getSize(); i++)
        result->setValue(i, getValue(i) / doubleValue );

    return result;
}