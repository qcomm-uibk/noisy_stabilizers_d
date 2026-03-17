/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef STATE_H
#define STATE_H

#pragma once

#include <vector>
#include "../logging/logger.h"
#include "../utils/matrix.h"
#include "../utils/matrixDoubles.h"
#include "../utils/matrixComplex.h"

using namespace std;

class State
{
public:
    State();
    State(MatrixInt *stabilizer, VectorInt *phase, int dimension);
    ~State();

    int getDimension();

    MatrixInt* getStabilizer();
    VectorInt* getPhase();

    bool IsCommuting(vector<int> row1, vector<int> row2);
    bool IsValid(vector<vector<int>> table);
    void Log(Logger *logger);

    bool isEqual(State *state);
    State* copy() { return new State(mStabilizer->copy(), mPhase->copy(), mDimension); }
    State* addRows(int row1, int row2);
    void addRowsInPlace(int row1, int row2);
    void addRowsMultipleInPlace(MatrixInt* stabilizer, VectorInt* phase, int row1, int row2, int factor);
    void swapRows(int row1, int row2);

    State* reduceTableauPostMeas(int idx);
    State* reduceSeparableTableau(VectorInt* subset);
    // Original name def_x_to_rref_mod_p(T, s, p=2)
    State* XtoRrefMod();

    void createPauliMatrices(MatrixInt* Z, MatrixInt* X, MatrixInt* id);
    MatrixComplex* pureStateDensityMatrix();
    MatrixInt* kroneckerProduct(MatrixInt* a, MatrixInt* b);
    MatrixComplex* kroneckerProduct(MatrixComplex* a, MatrixComplex b);
    

private:
    MatrixInt *mStabilizer;
    VectorInt *mPhase;

    int mDimension;
};

#endif