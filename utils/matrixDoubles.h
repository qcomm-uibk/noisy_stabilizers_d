/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef MATRIXDOUBLES_H
#define MATRIXDOUBLES_H

#pragma once

#include <vector>
#include <eigen3/unsupported/Eigen/MatrixFunctions>
#include <eigen3/Eigen/src/Eigenvalues/SelfAdjointEigenSolver.h>
#include "matrix.h"
#include "vectorDouble.h"
#include "../logging/logger.h"

using namespace std;

class MatrixDoubles
{
public:
    MatrixDoubles() {}
    MatrixDoubles(vector<vector<double>> matrix);
    MatrixDoubles(MatrixInt* matrix);
    MatrixDoubles(int rows, int cols, double initialValue);
    ~MatrixDoubles();

    double getValue(int i, int j) { return mMatrix[i][j]; } 
    void setValue(int i, int j, double value) { mMatrix[i][j] = value; }
    int getNumRows() { return mMatrix.size(); }
    int getColumnsOfRow(int i) { return mMatrix[i].size(); }
    void addRow(VectorDouble* newRow) { mMatrix.push_back(newRow->getVector());}

    MatrixDoubles* copy() { return new MatrixDoubles(mMatrix); }

    MatrixDoubles* multiplyByDouble(double value, bool createNewMatrix);
    MatrixDoubles* multiplyMatrices(MatrixDoubles* secondMatrix);
    MatrixDoubles* divisionByDouble(double value, bool createNewMatrix);
    MatrixDoubles* transpose(bool createNewMatrix);
    MatrixDoubles* sum(MatrixDoubles* secondMatrix, bool createNewMatrix);
    MatrixDoubles* sqrt(bool createNewMatrix);
    double trace();

    void logMatrix(Logger *logger);

private:
    vector<vector<double>> mMatrix;
};

#endif