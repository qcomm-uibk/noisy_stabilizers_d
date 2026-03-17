/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "mathUtils.h"
#include <stdexcept>

MathUtils::MathUtils()
{
}

MathUtils::~MathUtils()
{
}

long long MathUtils::mod(long long k, long long n)
{
    return ((k %= n) < 0) ? k+n : k;
}

int MathUtils::power(int value, int exponent)
{    
    if (exponent < 0)
        throw invalid_argument("MathUtils::power: The exponent can not be negative.");
    
    int result = value;

    if (exponent == 0)
        return 1;

    for (int i = 1; i < exponent; i++)
    {
        result = result * value;
    }

    return result;
}

int MathUtils::kronecker_delta(int a, int b)
{
    if (a == b)
        return 1;
    else
        return 0;
}

double MathUtils::getTolerance()
{
    return 1e-6;
}