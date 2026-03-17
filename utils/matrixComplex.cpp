/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "matrixComplex.h"
#include "mathUtils.h"

using Eigen::SelfAdjointEigenSolver;

MatrixComplex::MatrixComplex()
{
}

MatrixComplex::MatrixComplex(vector<vector<complex<double>>> matrix):
    mMatrix { matrix }
{
}

MatrixComplex::MatrixComplex(MatrixInt* matrix)
{
    for (int i = 0; i < matrix->getNumRows(); i++)
    {
        VectorComplex* newRow = new VectorComplex();
        for (int j = 0; j < matrix->getColumnsOfRow(i); j++)
            newRow->pushBack(matrix->getValue(i, j));
        addRow(newRow);
    }
}

MatrixComplex::MatrixComplex(int rows, int cols, complex<double> initialValue)
{
    for (int indexRows = 0; indexRows < rows; indexRows++)
    {
        vector<complex<double>> temp;
        for (int indexCols = 0; indexCols < cols; indexCols++)
            temp.push_back(initialValue);
        mMatrix.push_back(temp);
    }
}

MatrixComplex::~MatrixComplex()
{ 
}

MatrixComplex* MatrixComplex::sum(MatrixComplex* secondMatrix, bool createNewMatrix)
{
    // Check if they have the same dimension
    if (getNumRows() != secondMatrix->getNumRows())
        throw invalid_argument("MatrixComplex::sum: the matrices dimensions are different");

    MatrixComplex* result;

    if (createNewMatrix)
        result = new MatrixComplex();
    else
        result = this;
    
    for (int row = 0; row < getNumRows(); row++)
    {
        // Check dimension
        int numCols = getColumnsOfRow(row);
        if (numCols != secondMatrix->getColumnsOfRow(row))
            throw invalid_argument("MatrixComplex::sum: the matrices dimensions are different");

        if (createNewMatrix)
        {
            VectorComplex* newRow = new VectorComplex();
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

MatrixComplex* MatrixComplex::multiplyMatrices(MatrixComplex* secondMatrix)
{
    MatrixComplex* result = new MatrixComplex();

    for (int i = 0; i < getNumRows(); i++)
    {
        VectorComplex* newRow = new VectorComplex();
        for (int j = 0; j < secondMatrix->getColumnsOfRow(0); j++)
        {
            complex<double> sum = 0;
            for (int k = 0; k < secondMatrix->getNumRows(); k++)
                sum += getValue(i, k) * secondMatrix->getValue(k,j);
            newRow->pushBack(sum);
        }
        result->addRow(newRow);
    }
    return result;
}

MatrixComplex* MatrixComplex::divisionByDouble(double value, bool createNewMatrix)
{
    MatrixComplex* result;
    if (createNewMatrix)
        result = copy();
    else
        result = this;

    for (int i = 0; i < mMatrix.size(); i++)
        for (int j = 0; j < mMatrix[i].size(); j++)
            result->setValue(i, j, mMatrix[i][j] / value);
    
    return result;
}

MatrixComplex* MatrixComplex::multiplyByComplex(complex<double> value, bool createNewMatrix)
{
    MatrixComplex* result;

    if (createNewMatrix)
        result = copy();
    else
        result = this;

    for (int i = 0; i < mMatrix.size(); i++)
        for (int j = 0; j < mMatrix[i].size(); j++)
            result->setValue(i, j, mMatrix[i][j] * value);
    
    return result;
}

MatrixComplex* MatrixComplex::multiplyByDouble(double value, bool createNewMatrix)
{
    return multiplyByComplex(value, createNewMatrix);
}

MatrixComplex* MatrixComplex::matrixExp(int value)
{
    // Matrix is empty
    if (getNumRows() == 0)
        throw invalid_argument("MatrixComplex::matrixExp: the matrix is empty");
    
    if (value == 0 && getNumRows() == 2 && getColumnsOfRow(0) == 2)
    {
        MatrixComplex* identityMatrix = new MatrixComplex();
        identityMatrix->addRow(new VectorComplex({1,0}));
        identityMatrix->addRow(new VectorComplex({0,1}));
        return identityMatrix;
    }
    else if (value == 0 && getNumRows() == getColumnsOfRow(0))
    {
        return new MatrixComplex(MatrixInt::createIdentityMatrix(getNumRows()));
    }

    // Use multiplyMatrices
    MatrixComplex* result = copy();

    for (int i = 1; i < value; i++)
        result = result->multiplyMatrices(this);

    return result;
}

MatrixComplex* MatrixComplex::transpose(bool createNewMatrix)
{
    int newNumRows = getColumnsOfRow(0);
    int newNumCols = getNumRows();

    if (createNewMatrix)
    {
        MatrixComplex* result = new MatrixComplex(newNumRows, newNumCols, 0);

        for (int i = 0; i < newNumRows; i++)
            for (int j = 0; j < newNumCols; j++)
                result->setValue(i, j, mMatrix[j][i]);
    
        return result;
    }
    else
    {
        MatrixComplex* copied = copy();

        for (int i = 0; i < newNumRows; i++)
            for (int j = 0; j < newNumCols; j++)
                this->setValue(i, j, copied->getValue(j,i));

        delete copied;
        return this;
    }
}

MatrixComplex* MatrixComplex::complexConjugate(bool createNewMatrix)
{
    MatrixComplex* result; 
    if (createNewMatrix)
        result = copy();
    else
        result = this;

    for(int i = 0; i < result->getNumRows(); i++)
        for(int j = 0; j < result->getColumnsOfRow(i); j++)
            result->setValue(i, j, complex<double>(result->getValue(i,j).real(), -result->getValue(i,j).imag()));
    return result;
}

MatrixComplex* MatrixComplex::sqrt(bool createNewMatrix)
{
    // To be able to calculate the square root of a matrix, it needs to be square
    if (getNumRows() != getColumnsOfRow(0))
        throw invalid_argument("MatrixComplex::sqrt: the matrices are not the same dimension");

    int size = getNumRows();

    // Translate the matrix to a format Eigen can use
    Eigen::MatrixXcd eigenMatrix(size, size);

    // Add values to the eigenMatrix
    for (int row = 0; row < size; row++)
        for (int col = 0; col < size; col++)
            eigenMatrix(row, col) = mMatrix[row][col];

    SelfAdjointEigenSolver<Eigen::MatrixXcd> es(eigenMatrix);
    Eigen::MatrixXcd sqrtEigenResult = es.operatorSqrt();

    MatrixComplex* result;

    if (createNewMatrix) 
    {
        result = new MatrixComplex();
        // Translate back to our matrix system
        for (int row = 0; row < size; row++)
        {
            VectorComplex* newRow = new VectorComplex();
            for (int col = 0; col < size; col++)
                newRow->pushBack(eigenMatrix(row, col));
            result->addRow(newRow);
        }
    }
    else 
    {
        result = this;
        // Translate back to our matrix system
        for (int row = 0; row < size; row++)
            for (int col = 0; col < size; col++)
                result->setValue(row,col,eigenMatrix(row, col));        
    }    

    return result;
}

complex<double> MatrixComplex::trace()
{
    // Needs to be a square matrix
    if (getNumRows() != getColumnsOfRow(0))
        throw invalid_argument("MatrixComplex::trace: the matrix should be squared.");

    complex<double> result(0,0);

    // Sum elements in the diagonal
    for(int i = 0; i < getNumRows(); i++)
        result += mMatrix[i][i];

    return result;
}

// Log function!
void MatrixComplex::logMatrix(Logger *logger)
{
    int numRows = getNumRows();
    for (int i = 0; i < numRows; i++)
    {        
        int numCols = getColumnsOfRow(i);

        string line("");
        for(int j = 0; j < numCols; j++)
        {
            double real = getValue(i,j).real();
            double imag = getValue(i,j).imag();

            string number("");
            if(imag == 0.0)
                number = to_string(real);
            else {
                if(imag < 0)
                    number = to_string(real) + to_string(imag) + "j"; 
                else
                    number = to_string(real) + "+" + to_string(imag) + "j"; 
            }
            if(j==0)
            {
                line = number;
            }
            else
            {
                line = line + "," + number;
            }
        }
        
        logger->WriteLog(line);     
    }
}


bool MatrixComplex::isEqual(MatrixComplex* other)
{    
    if(getNumRows() != other->getNumRows())
        return false;

    for (int i = 0; i < getNumRows(); i++)
    {
        int cols1 = getColumnsOfRow(i);

        if(cols1 != other->getColumnsOfRow(i))
            return false;

        for (int j = 0; j < cols1; j++)
        {
            complex<double> z1 = getValue(i,j);
            complex<double> z2 = other->getValue(i,j);

            double difference = std::abs(z1 - z2);
            if(difference > MathUtils::getTolerance())
                return false;
        }
    }

    return true;
}