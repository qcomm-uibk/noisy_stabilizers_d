/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "matrixDoubles.h"

using Eigen::SelfAdjointEigenSolver;

MatrixDoubles::MatrixDoubles(vector<vector<double>> matrix):
    mMatrix{ matrix }
{    
}

MatrixDoubles::MatrixDoubles(MatrixInt* matrix)
{
    for (int i = 0; i < matrix->getNumRows(); i++)
    {
        vector<double> temp;
        for (int j = 0; j < matrix->getColumnsOfRow(i); j++)
            temp.push_back(matrix->getValue(i, j));
        mMatrix.push_back(temp);
    }
}

MatrixDoubles::MatrixDoubles(int rows, int cols, double initialValue)
{
    for(int rowsIndex = 0; rowsIndex < rows; rowsIndex++)
    {
        vector<double> temp;
        for (int colsIndex = 0; colsIndex < cols; colsIndex++)
            temp.push_back(initialValue);
        mMatrix.push_back(temp);
    }
}

MatrixDoubles::~MatrixDoubles()
{
}

MatrixDoubles* MatrixDoubles::multiplyMatrices(MatrixDoubles* secondMatrix)
{
    MatrixDoubles* result = new MatrixDoubles();
    
    for (int i = 0; i < getNumRows(); i++)
    {
        VectorDouble* newRow = new VectorDouble();
        for (int j = 0; j < secondMatrix->getColumnsOfRow(0); j++)
        {
            double sum = 0;
            for (int k = 0; k < secondMatrix->getNumRows(); k++)
                sum += getValue(i, k) * secondMatrix->getValue(k,j);
            newRow->append(sum);
        }
        result->addRow(newRow);
    }
    return result;
}

MatrixDoubles* MatrixDoubles::divisionByDouble(double value, bool createNewMatrix)
{
    MatrixDoubles* result;

    if (createNewMatrix)
        result = copy();
    else
        result = this;

    for (int i = 0; i < mMatrix.size(); i++)
        for (int j = 0; j < mMatrix[i].size(); j++)
            result->setValue(i, j, mMatrix[i][j] / value);
    
    return result;
}

MatrixDoubles* MatrixDoubles::transpose(bool createNewMatrix)
{
    int newNumRows = getColumnsOfRow(0);
    int newNumCols = getNumRows();

    if (createNewMatrix)
    {
        MatrixDoubles* result = new MatrixDoubles(newNumRows, newNumCols, 0);

        for (int i = 0; i < newNumRows; i++)
            for (int j = 0; j < newNumCols; j++)
                result->setValue(i, j, mMatrix[j][i]);
    
        return result;
    }
    else
    {
        MatrixDoubles* copied = copy();

        for (int i = 0; i < newNumRows; i++)
            for (int j = 0; j < newNumCols; j++)
                this->setValue(i, j, copied->getValue(j,i));

        delete copied;
        return this;
    }
}

MatrixDoubles* MatrixDoubles::sum(MatrixDoubles* secondMatrix, bool createNewMatrix)
{
    // Check if they have the same dimension
    if (getNumRows() != secondMatrix->getNumRows())
        throw invalid_argument("MatrixDoubles::sum: the matrices are not the same dimension");

    MatrixDoubles* result;

    if (createNewMatrix)
        result = new MatrixDoubles();
    else
        result = this;

    for (int row = 0; row < getNumRows(); row++)
    {
        // Check dimension
        int numCols = getColumnsOfRow(row);
        if (numCols != secondMatrix->getColumnsOfRow(row))
            throw invalid_argument("MatrixDoubles::sum: the matrices are not the same dimension");

        if (createNewMatrix)
        {
            VectorDouble* newRow = new VectorDouble();
            for (int col = 0; col < numCols; col++)
                newRow->append(getValue(row, col) + secondMatrix->getValue(row, col));
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

MatrixDoubles* MatrixDoubles::multiplyByDouble(double value, bool createNewMatrix)
{
    MatrixDoubles* result;

    if (createNewMatrix)
        result = copy();
    else
        result = this;

    for (int i = 0; i < mMatrix.size(); i++)
        for (int j = 0; j < mMatrix[i].size(); j++)
            result->setValue(i, j, mMatrix[i][j] * value);
    
    return result;
}

MatrixDoubles* MatrixDoubles::sqrt(bool createNewMatrix)
{
    // To be able to calculate the square root of a matrix, it needs to be square
    if (getNumRows() != getColumnsOfRow(0))
        throw invalid_argument("MatrixDoubles::sqrt: the matrices are not the same dimension");

    int size = getNumRows();

    // Translate the matrix to a format Eigen can use
    Eigen::MatrixXd eigenMatrix(size, size);

    // Add values to the eigenMatrix
    for (int row = 0; row < size; row++)
        for (int col = 0; col < size; col++)
            eigenMatrix(row, col) = mMatrix[row][col];

    SelfAdjointEigenSolver<Eigen::MatrixXd> es(eigenMatrix);
    Eigen::MatrixXd sqrtEigenResult = es.operatorSqrt();
    
    MatrixDoubles* sqrtResult;

    if (createNewMatrix)
    {
        sqrtResult = new MatrixDoubles();
        // Translate back to our matrix system
        for (int row = 0; row < size; row++)
        {
            VectorDouble* newRow = new VectorDouble();
            for (int col = 0; col < size; col++)
                newRow->append(sqrtEigenResult(row, col));
            sqrtResult->addRow(newRow);
        }
    }
    else
    {
        sqrtResult = this;
        // Translate back to our matrix system
        for (int row = 0; row < size; row++)
            for (int col = 0; col < size; col++)
                sqrtResult->setValue(row,col,eigenMatrix(row, col));
    }
    
    return sqrtResult;    
}

double MatrixDoubles::trace()
{
    // Needs to be a square matrix
    if (getNumRows() != getColumnsOfRow(0))
        throw invalid_argument("MatrixDoubles::trace: the matrix should be squared.");

    double result = 0;

    // Sum elements in the diagonal
    for(int i = 0; i < getNumRows(); i++)
        result += mMatrix[i][i];

    return result;
}

//log function!
void MatrixDoubles::logMatrix(Logger *logger)
{
    int numRows = getNumRows();
    for (int i = 0; i < numRows; i++)
    {        
        int numCols = getColumnsOfRow(i);

        string line("");
        for(int j = 0; j < numCols; j++)
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