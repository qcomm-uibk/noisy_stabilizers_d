/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef MATRIXCOMPLEX_H
#define MATRIXCOMPLEX_H

#pragma once

#include <eigen3/unsupported/Eigen/MatrixFunctions>
#include <eigen3/Eigen/src/Eigenvalues/SelfAdjointEigenSolver.h>
#include "vectorComplex.h"
#include "matrix.h"
#include "matrixDoubles.h"
#include "../logging/logger.h"

using namespace std;

class MatrixComplex
{
public:
    MatrixComplex();
    MatrixComplex(vector<vector<complex<double>>> matrix);
    MatrixComplex(MatrixInt* matrix);
    MatrixComplex(int rows, int cols, complex<double> initialValue);
    ~MatrixComplex();

    complex<double> getValue(int i, int j) { return mMatrix[i][j]; } 
    void setValue(int i, int j, complex<double> value) { mMatrix[i][j] = value; }
    int getNumRows() { return mMatrix.size(); }
    int getColumnsOfRow(int i) { return mMatrix[i].size(); }
    void addRow(VectorComplex* newRow) { mMatrix.push_back(newRow->getVector());}
    MatrixComplex* copy() { return new MatrixComplex(mMatrix); }

    MatrixComplex* sum(MatrixComplex* secondMatrix, bool createNewMatrix);
    MatrixComplex* multiplyMatrices(MatrixComplex* secondMatrix);
    MatrixComplex* divisionByDouble(double value, bool createNewMatrix);
    MatrixComplex* multiplyByComplex(complex<double> value, bool createNewMatrix);
    MatrixComplex* multiplyByDouble(double value, bool createNewMatrix);
    MatrixComplex* transpose(bool createNewMatrix);
    MatrixComplex* matrixExp(int value);
    MatrixComplex* complexConjugate(bool createNewMatrix);
    MatrixComplex* sqrt(bool createNewMatrix);
    complex<double> trace();

    bool isEqual(MatrixComplex* other);

    void logMatrix(Logger *logger);

private:
    vector<vector<complex<double>>> mMatrix;

};

#endif