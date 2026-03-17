/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef PARSEUTILS_H
#define PARSEUTILS_H

#include <vector>
#include <string>

#pragma once

using namespace std;

class ParseUtils
{
public:
    ParseUtils();
    ~ParseUtils();

    static vector<int> split(string s, string delimiter);
    static vector<complex<double>> splitComplex(string s, string delimiter);

private:
    static complex<double> parseComplex(string token);
};

#endif