/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef MATRIX_H
#define MATRIX_H

#pragma once

#include <vector>
#include "../logging/logger.h"
#include "mathUtils.h"
#include "vectorint.h"

using namespace std;

class MatrixInt
{
public:
    MatrixInt() {}
    MatrixInt(vector<vector<int>> matrix);
    MatrixInt(int rows, int cols, int initialValue); 
    ~MatrixInt();

    static MatrixInt* createIdentityMatrix(int n);

    int getValue(int i, int j) { return mMatrix[i][j]; } 
    void setValue(int i, int j, int value) { mMatrix[i][j] = value; }
    void setValueMod(int i, int j, int value, int d) { mMatrix[i][j] = MathUtils::mod(value, d); }
    int getNumRows() { return mMatrix.size(); }
    int getColumnsOfRow(int i) { return mMatrix[i].size(); }
    VectorInt* getRow(int i) { 
        vector<int> row;
        for(int j=0; j<getColumnsOfRow(i); j++)
            row.push_back(getValue(i,j));

        return new VectorInt(row);
    }
    VectorInt* getFullColumn(int col);
    MatrixInt* getSelectedElements(VectorInt* rows, VectorInt* cols); // np.ix_()
    int indexMaxValueInCol(int col, int startRow = 0);

    void add(int i, int j, int value) { mMatrix[i][j] += value; }
    void addRow(VectorInt* newRow) { mMatrix.push_back(newRow->getVector()); }
    
    void deleteValue(int row, int col);
    void removeRow(int row);
    void removeColumn(int col);

    MatrixInt* copy() { return new MatrixInt(mMatrix); }
    bool isEqual(MatrixInt* other);
    void swapRows(int row1, int row2);
    MatrixInt* slice(int startPosRow, int stopPosRow, int startPosCol, int stopPosCol);
    void logMatrix(Logger* logger);

    void mod(int d) {
        for(int i=0; i<getNumRows(); i++)
        {
            for(int j=0; j<getColumnsOfRow(i); j++)
            {
                mMatrix[i][j] = MathUtils::mod(mMatrix[i][j], d);
            }
        }
    }
    void addAndMod(int i, int j, int value, int d) { setValueMod(i, j, mMatrix[i][j] + value, d); }
    VectorInt* getMultipliedRow(int row, int factor);
    MatrixInt* multiplyMatrices(MatrixInt* secondMatrix);
    MatrixInt* multiplyInt(int value, bool createNewMatrix);
    MatrixInt* matrixExp(int value);
    MatrixInt* sum(MatrixInt* secondMatrix, bool createNewMatrix);

private:
    vector<vector<int>> mMatrix;
};

#endif