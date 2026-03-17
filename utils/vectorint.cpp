/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "vectorint.h"
#include <stdexcept>
#include <algorithm>

VectorInt::VectorInt()
{

}

VectorInt::VectorInt(vector<int> vector):
    mVector { vector }
{

}

VectorInt::~VectorInt()
{

}

void VectorInt::initialize(int numElements, int initialValue)
{
    mVector = {};
    for (int i = 0; i < numElements; i++)
        mVector.push_back(initialValue);
}

VectorInt* VectorInt::nonzeroIndx()
{
    VectorInt* vectorToReturn = new VectorInt();
    for(int i=0; i < mVector.size(); i++)
    {
        if (mVector.at(i)== 0)
            continue;
        else
            vectorToReturn->pushBack(i);
    }
    return vectorToReturn;
}

void VectorInt::concatenateAsSet(VectorInt* secondVector)
{
    for(int i=0; i < secondVector->getSize(); i++)
        if (std::find(mVector.begin(), mVector.end(), secondVector->getValue(i)) == mVector.end())
            mVector.push_back(secondVector->getValue(i));
}

bool VectorInt::contains(int value)
{
    for(int val : mVector)
    {
        if (val != value)
            continue;
        else
            return true;
    }
    return false;
}

VectorInt* VectorInt::sum(VectorInt* secondVector)
{
    if (getSize() != secondVector->getSize())
        throw invalid_argument("VectorInt::sum: Vectors have different lenghts.");

    VectorInt* result = new VectorInt();
    for (int i = 0; i < getSize(); i++)
        result->pushBack(getValue(i) + secondVector->getValue(i));

    return result;
}

VectorInt* VectorInt::notVector()
{
    std::vector<int> newVector(getSize());

    for(int i = 0; i < getSize(); i++)
        newVector[i] = -getValue(i);

    return new VectorInt(newVector); 
}

VectorInt* VectorInt::multiplyVectorInt(int value)
{
    std::vector<int> newVector(getSize());

    for (int i = 0; i < getSize(); i++)
        newVector[i] = getValue(i) * value;

    return new VectorInt(newVector);
}

int VectorInt::gamma()
{
    int n = getSize() / 2;
    int aux = 0;

    // A scalar product between the z-part and the x-part
    for (int i = 0; i < n; i++)
        aux += getValue(i) * getValue(i+n);

    return MathUtils::mod(aux, 4);
}

int VectorInt::beta(VectorInt* other)
{
    int n = getSize() / 2;
    
    VectorInt* newRow = new VectorInt();

    for (int j = 0; j < getSize(); j++)
        newRow->pushBack(MathUtils::mod((getValue(j) + other->getValue(j)), 2));

    int firstPart = newRow->gamma() - gamma() - other->gamma();
    int secondPart = 0;

    for (int i = 0; i < n; i++)
        secondPart += getValue(i) * other->getValue(i+n);
    
    secondPart = 2 * secondPart;

    return MathUtils::mod((firstPart + secondPart), 4);
}

int VectorInt::sigma(VectorInt* w, int dimension)
{
    int m = getSize();
    // TODO Julius: What happens when size is not even and we divide by 2??
    int n = m/2;
    int sig = 0;

    // TODO Julius: Also here, what happens if n was rounded up or down?
    for (int i = 0; i < n; i++)
        sig += (getValue(i) * w->getValue(i+n)) - (w->getValue(i)*getValue(i+n));
    
    sig = MathUtils::mod(sig, dimension);
    return sig;
}

VectorInt* VectorInt::mod(int modValue)
{
    VectorInt* result = new VectorInt();
    for (int i = 0; i < getSize(); i++)
        result->pushBack(MathUtils::mod(getValue(i), modValue));

    return result;
}

void VectorInt::logVector(Logger *logger)
{
    string line("");
    for (int i = 0; i < getSize(); i++)
    {
        if(i==0)
        {
            line = to_string(getValue(i));
        }
        else
        {
            line = line + "," + to_string(getValue(i));
        }
    }
    logger->WriteLog(line);
}

bool VectorInt::isEqual(VectorInt* other)
{
    if (getSize() != other->getSize())
        return false;
    
    for (int i = 0; i < getSize(); i++)
        if (getValue(i) != other->getValue(i))
            return false;
    return true;
}

void VectorInt::swapElements(int index1, int index2)
{
    int temp = mVector[index1];
    mVector[index1] = mVector[index2];
    mVector[index2] = temp;
}

VectorInt* VectorInt::getSelectedElements(VectorInt* indices)
{
    VectorInt* result = new VectorInt();
    for (int i = 0; i < indices->getSize(); i++)
        result->pushBack(getValue(indices->getValue(i)));
    return result;
}