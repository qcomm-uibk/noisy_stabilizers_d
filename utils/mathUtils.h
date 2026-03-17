/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef MATHUTILS_H
#define MATHUTILS_H

#pragma once

#include <vector>

using namespace std;
class MathUtils
{
public:
    MathUtils();
    ~MathUtils();

    static long long mod(long long k, long long n);
    static int power(int value, int exponent);
    static int kronecker_delta(int a, int b);

    static double getTolerance();

private:

};

#endif