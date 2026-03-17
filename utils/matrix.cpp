/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#include "matrix.h"
#include <stdexcept>

MatrixInt::MatrixInt(vector<vector<int>> matrix):
    mMatrix{ matrix }
{    
}

MatrixInt::MatrixInt(int rows, int cols, int initialValue)
{
    for (int indexRows = 0; indexRows < rows; indexRows++)
    {
        vector<int> temp;
        for (int indexCols = 0; indexCols < cols; indexCols++)
            temp.push_back(initialValue);
        mMatrix.push_back(temp);
    }
}

MatrixInt::~MatrixInt()
{
}

bool MatrixInt::isEqual(MatrixInt* other)
{
    if(getNumRows() != other->getNumRows())
        return false;

    for (int i = 0; i < getNumRows(); i++)
    {
        int cols1 = getColumnsOfRow(i);

        if(cols1 != other->getColumnsOfRow(i))
            return false;

        for (int j = 0; j < cols1; j++)
            if(getValue(i,j) != other->getValue(i,j))
                return false;
    }

    return true;
}

void MatrixInt::deleteValue(int row, int col)
{
    mMatrix[row].erase(mMatrix[row].begin() + col);
}

void MatrixInt::removeRow(int row)
{
    if (row >= mMatrix.size())
        return;
    mMatrix.erase(mMatrix.begin() + row);
}

void MatrixInt::removeColumn(int col)
{
    if (col >= mMatrix.at(0).size())
        return;
    for (int row = 0; row < mMatrix.size(); row++)
        mMatrix[row].erase(mMatrix[row].begin()+col);
}

VectorInt* MatrixInt::getMultipliedRow(int rowIndex, int factor)
{ 
    vector<int> row;
    for(int j=0; j<getColumnsOfRow(rowIndex); j++)
        row.push_back(getValue(rowIndex,j) * factor);

    return new VectorInt(row);
}

void MatrixInt::swapRows(int row1, int row2)
{
    vector<int> temp = mMatrix[row1];
    mMatrix[row1] = mMatrix[row2];
    mMatrix[row2] = temp;
}

MatrixInt* MatrixInt::slice(int startPosRow, int stopPosRow, int startPosCol, int stopPosCol)
{
    if (startPosRow < 0 || stopPosRow < 1 || startPosCol < 0 || stopPosCol < 1)
        throw invalid_argument("Matrix::slice: invalid argument values.");

    MatrixInt* slicedMatrix = new MatrixInt();

    for (int row = startPosRow; row < stopPosRow; row++)
    {
        VectorInt* newRow = new VectorInt();
        for (int col = startPosCol; col < stopPosCol; col++)
        {
            newRow->pushBack(getValue(row, col));
        }
        slicedMatrix->addRow(newRow);
    }
    return slicedMatrix;
}

int MatrixInt::indexMaxValueInCol(int col, int startRow)
{
    if (col > getColumnsOfRow(startRow) || col < 0 || startRow < 0)
        throw invalid_argument("Matrix::indexMaxValueInCol: invalid argument values.");
    
    int returnIndex = -1;
    int value = -1;
    
    for (int row = startRow; row < getNumRows(); row++)
    {
        if (getValue(row, col) > value)
        {
            value = getValue(row, col);
            returnIndex = row;
        }
    }

    //careful here, the starting row must be subtracted since we get the value of X[pivot_row:, col] --> the argmax is done over the 
    //sliced vector (!!!)
    if(returnIndex != -1)
        return returnIndex - startRow;

    return returnIndex;
}

VectorInt* MatrixInt::getFullColumn(int col)
{
    VectorInt* vectorToReturn = new VectorInt();
    for(int i=0; i < getNumRows(); i++)
        vectorToReturn->pushBack(mMatrix[i][col]);
    return vectorToReturn;
}

MatrixInt* MatrixInt::getSelectedElements(VectorInt* rows, VectorInt* cols)
{
    MatrixInt* matrixToReturn = new MatrixInt();
    
    for(int auxRows = 0; auxRows < rows->getSize(); auxRows++)
    {
        VectorInt* newRow = new VectorInt();
        for(int auxCols = 0; auxCols < cols->getSize(); auxCols++)
            newRow->pushBack( mMatrix[rows->getValue(auxRows)][cols->getValue(auxCols)]);
        matrixToReturn->addRow(newRow);
    }
        
    return matrixToReturn;
}

MatrixInt* MatrixInt::multiplyMatrices(MatrixInt* secondMatrix)
{
    MatrixInt* result = new MatrixInt();
    
    for (int i = 0; i < getNumRows(); i++)
    {
        VectorInt* newRow = new VectorInt();
        for (int j = 0; j < secondMatrix->getColumnsOfRow(0); j++)
        {
            int sum = 0;
            for (int k = 0; k < secondMatrix->getNumRows(); k++)
                sum += getValue(i, k) * secondMatrix->getValue(k,j);
            newRow->pushBack(sum);
        }
        result->addRow(newRow);
    }
    return result;
}

MatrixInt* MatrixInt::multiplyInt(int value, bool createNewMatrix)
{
    MatrixInt* result;

    if (createNewMatrix)
        result = copy();
    else
        result = this;

    for (int i = 0; i < mMatrix.size(); i++)
        for (int j = 0; j < mMatrix[i].size(); j++)
            result->setValue(i, j, mMatrix[i][j] * value);
    
    return result;
}

MatrixInt* MatrixInt::matrixExp(int value)
{
    // Matrix is empty
    if (this->getNumRows() == 0)
        throw invalid_argument("Matrix::matrixExp: the matrix is empty");
    
    if (value == 0 && this->getNumRows() == 2 && this->getColumnsOfRow(0) == 2)
    {
        MatrixInt* identityMatrix = new MatrixInt();
        identityMatrix->addRow(new VectorInt({1,0}));
        identityMatrix->addRow(new VectorInt({0,1}));
        return identityMatrix;
    }
    else if (value == 0 && this->getNumRows() == this->getColumnsOfRow(0))
    {
        return MatrixInt::createIdentityMatrix(this->getNumRows());
    }

    // Use multiplyMatrices
    MatrixInt* result = copy();

    for (int i = 1; i < value; i++)
        result = result->multiplyMatrices(this);

    return result;
}

MatrixInt* MatrixInt::sum(MatrixInt* secondMatrix, bool createNewMatrix)
{
    // Check if they have the same dimension
    if (getNumRows() != secondMatrix->getNumRows())
        throw invalid_argument("Matrix::sum: the matrices dimensions are different");

    MatrixInt* result;

    if (createNewMatrix)
        result = new MatrixInt();
    else
        result = this;

    for (int row = 0; row < getNumRows(); row++)
    {
        // Check dimension
        int numCols = getColumnsOfRow(row);
        if (numCols != secondMatrix->getColumnsOfRow(row))
            throw invalid_argument("Matrix::sum: the matrices dimensions are different");

        if (createNewMatrix)
        {
            VectorInt* newRow = new VectorInt();
            for (int col = 0; col < numCols; col++)
                newRow->pushBack(getValue(row, col) + secondMatrix->getValue(row, col));
            result->addRow(newRow);
        }
        else
        {
            for (int col = 0; col < numCols; col++)
                result->setValue(row, col, getValue(row, col) + secondMatrix->getValue(row, col));
        }
    }
    return result;
}

void MatrixInt::logMatrix(Logger *logger)
{
    for (int i = 0; i < getNumRows(); i++)
    {   
        string line("");
        for(int j = 0; j < getColumnsOfRow(i); j++)
        {
            if(j==0)
            {
                line = to_string(getValue(i,j));
            }
            else
            {
                line = line + "," + to_string(getValue(i,j));
            }
        }
        
        logger->WriteLog(line);     
    }
}

MatrixInt* MatrixInt::createIdentityMatrix(int n)
{
    MatrixInt* identityMatrix = new MatrixInt(n,n,0);
    for(int i=0; i<n; i++)
        identityMatrix->setValue(i,i,1);

    return identityMatrix;
}