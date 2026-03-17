/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "state.h"
#include "../utils/mathUtils.h"

State::State()
{
}

State::State(MatrixInt* stabilizer, VectorInt* phase, int dimension)
{
    mStabilizer = stabilizer;
    mPhase = phase;
    mDimension = dimension;
}

State::~State()
{
    delete mStabilizer;
    delete mPhase;
}

void State::Log(Logger *logger)
{
    logger->WriteLog("Stabilizers:");
    mStabilizer->logMatrix(logger);

    if(mPhase->getSize() > 0)
    {
        logger->WriteLog("Phase:");
        mPhase->logVector(logger);
    }
}

MatrixInt* State::getStabilizer()
{
    return mStabilizer;
}

VectorInt* State::getPhase()
{
    return mPhase;
}

int State::getDimension()
{
    return mDimension;
}

bool State::IsCommuting(vector<int> row1, vector<int> row2)
{
    int n = sizeof(row1) / 2;
    int res = 0;

    for (int i = 0; i < n ; i++)
        res += (row1[i] * row2[i+n]) - (row1[i+n] * row2[i]);

    if (MathUtils::mod(res, mDimension) == 0)
        return true;
    else
        return false;
}

bool State::IsValid(vector<vector<int>> table)
{
    int numRows = sizeof(table);

    // Number of columns must be 2*numRows
    if (sizeof(table[0]) != 2*numRows)
        return false;
    
    for (int i = 0; i < numRows; i++)
    {
        vector<int> row_i = table[i];
        for (int j = i; j < numRows; j++)
        {
            vector<int> row_j = table[j];
            if (!IsCommuting(row_i, row_j))
                return false;
        } 
    }

    return true;
}

bool State::isEqual(State *other)
{
    //TODO rework equality for state permutations and noise maps/noisy states

    return 
        mStabilizer->isEqual(other->mStabilizer) && 
        mPhase->isEqual(other->mPhase); 
}

State* State::reduceTableauPostMeas(int idx)
{
    int numRows = getStabilizer()->getNumRows();

    // Step 1: Find index of the isolated stabilizer operator
    int j = -1;
    for (int i = 0; i < numRows; i++)
    {
        bool iFoundIt = true;
        for (int col = 0; col < numRows*2; col++)
        {
            if (getStabilizer()->getValue(i,col) - MathUtils::kronecker_delta(idx, col) != 0)
            {
                iFoundIt = false;
                break;
            }
        }
        if (!iFoundIt)
            break;
        else
        {
            j = i;
            break;
        }
    }

    if (j == -1)
        throw new invalid_argument("State::reduceTableauPostMeas: isolated stabilizer operator (j) not found");

    // Step 2: Remove local Z stabilizer terms from other stabilizers
    MatrixInt* newStabilizer = getStabilizer()->copy();
    VectorInt* newPhase = getPhase()->copy();

    for(int i=0; i < numRows; i++)
    {
        if (getStabilizer()->getValue(i, idx) == 1)
        {
            for (int colStab = 0; colStab < newStabilizer->getColumnsOfRow(i); colStab++)
            {
                // Element-wise adition of rows i and j
                int value = newStabilizer->getValue(i, colStab) + newStabilizer->getValue(j, colStab);
                newStabilizer->setValue(i, colStab, value);
            }

            int valueBeta = newStabilizer->getRow(i)->beta(newStabilizer->getRow(j))/2;
            int valuePhase = newPhase->getValue(i) + newPhase->getValue(j) + valueBeta;
            newPhase->setValue(i, valuePhase);
            
        }
    }

    // Step 3: Remove jth row and cols idx, idx+numRows of stabilizer and the jth entry of the phase
    newStabilizer->removeRow(j);
    newStabilizer->removeColumn(idx);
    newStabilizer->removeColumn(idx+numRows-1);
    newPhase->removeElement(j);

    return new State(newStabilizer, newPhase, getDimension());
}

State* State::addRows(int row1, int row2)
{
    MatrixInt* newStabilizer = getStabilizer()->copy();
    VectorInt* newPhase = getPhase()->copy();

    if (row1 >= newStabilizer->getNumRows() || row2 >= newStabilizer->getNumRows())
        throw new invalid_argument("State::addRows: the number of rows is bigger than the matrix size");

    for (int col = 0; col < newStabilizer->getColumnsOfRow(row2); col++)
    {
        int newValue = newStabilizer->getValue(row2, col) + newStabilizer->getValue(row1, col);
        newStabilizer->setValue(row2, col, newValue); 
    }

    if (mDimension == 2)
    {
        int valueBeta = getStabilizer()->getRow(row1)->beta(getStabilizer()->getRow(row2)) / 2;        
        int newValue = newPhase->getValue(row1) + newPhase->getValue(row2) + valueBeta;
        newPhase->setValue(row2, newValue);  
    }    
    else
    {
        int newValue = newPhase->getValue(row1) + newPhase->getValue(row2);
        newPhase->setValue(row2, newValue);
    }
        
    newStabilizer->mod(mDimension);
    newPhase->mod(mDimension);

    return new State(newStabilizer, newPhase, mDimension);
}

void State::addRowsInPlace(int row1, int row2)
{
    // Add the first row to the second row without creating another object
    if (mDimension == 2)
    {
        int valueBeta = getStabilizer()->getRow(row1)->beta(getStabilizer()->getRow(row2)) / 2;
        int newValue = getPhase()->getValue(row1) + getPhase()->getValue(row2) + valueBeta;
        getPhase()->setValue(row2, MathUtils::mod(newValue, 2));
    }
        
    else
    {
        int newValue = getPhase()->getValue(row1) + getPhase()->getValue(row2);
        getPhase()->setValue(row2, MathUtils::mod(newValue, mDimension));
    }
    
    for (int col = 0; col < getStabilizer()->getColumnsOfRow(row2); col++)
    {
        int newValue = getStabilizer()->getValue(row1, col) + getStabilizer()->getValue(row2, col);
        getStabilizer()->setValue(row2, col, MathUtils::mod(newValue, mDimension)); 
    }
}

void State::addRowsMultipleInPlace(MatrixInt* stabilizer, VectorInt* phase, int row1, int row2, int factor)
{
    // Add the factor-multiple of row1 to row2
    if (mDimension == 2)
    {
        VectorInt* partial = stabilizer->getMultipliedRow(row1, factor)->mod(2);
        int valueBeta = partial->beta(stabilizer->getRow(row2)) / 2;
        int newValue = MathUtils::mod(factor * phase->getValue(row1), 2) + phase->getValue(row2) +  valueBeta;
        phase->setValue(row2, MathUtils::mod(newValue, 2));
        delete partial;
    }
    else
    {
        int newValue = factor * phase->getValue(row1) + phase->getValue(row2);
        phase->setValue(row2, MathUtils::mod(newValue, mDimension));
    }

    for (int col = 0; col < stabilizer->getColumnsOfRow(row2); col++)
    {
        int newValue = factor * stabilizer->getValue(row1, col) + stabilizer->getValue(row2, col);
        stabilizer->setValue(row2, col, MathUtils::mod(newValue, mDimension)); 
    }
}



void State::swapRows(int row1, int row2)
{
    getStabilizer()->swapRows(row1, row2);
    getPhase()->swapElements(row1, row2);
}

State* State::XtoRrefMod()
{
    /*
    Pick a representation of Stabilizer generators that is nearly a graph State.
    This essentially performs Gaussian elimination on the X part of the Matrix T=(Z|X)
    Why not use existing algorithms?
    a) Simpy, scipy do not have a variant of the algorithms mod p and just applying the mod
       afterwards does not work in all cases, i.e. you may end up with division by 0 (maybe some
       finite fields libraries may have this, but I did not find anything quickly)
    b) Most algorithms I found to bring something to reduced row echelon formm do not give the associated
       transformation back. But this is necessary to compute the correct s for even dimension.
    */ 
   MatrixInt* newStabilizer = getStabilizer()->copy();
   VectorInt* newPhase = getPhase()->copy();
   int numRows = newStabilizer->getNumRows();
   
   // Check the size is correct
   if (newStabilizer->getColumnsOfRow(0) != 2* numRows)
        throw new invalid_argument("State::XtoRrefMod: Stabilizer size is not correct.");
   
    //all operations below here are done on the newStabilizer (according to JW)

   // X = T[:,n:]
   MatrixInt* matrixX = newStabilizer->slice(0, getStabilizer()->getNumRows(), 
                                    numRows, getStabilizer()->getColumnsOfRow(0));
   int pivot_row = 0;
   vector<int> nonPivotColumns;

   for (int column = 0; column < numRows; column++)
   {
        int rowIndex = matrixX->indexMaxValueInCol(column, pivot_row) + pivot_row;
        if (matrixX->getValue(rowIndex, column) == 0)
        {
            nonPivotColumns.push_back(column);
            continue;
        }

        newStabilizer->swapRows(pivot_row, rowIndex); 
        
        //updating stabilizer means we also need to update the X matrix, because its a VIEW on stabilizer (according to JW!!!!) 
        matrixX = newStabilizer->slice(0, getStabilizer()->getNumRows(), 
                                    numRows, getStabilizer()->getColumnsOfRow(0));

        newPhase->swapElements(pivot_row, rowIndex);
        
        int inverseFactor = MathUtils::mod(MathUtils::power(matrixX->getValue(pivot_row, column), getDimension()-2), getDimension());
        
        for (int i = 0; i < newStabilizer->getColumnsOfRow(pivot_row); i++)
            newStabilizer->setValue(pivot_row, i, MathUtils::mod(inverseFactor * newStabilizer->getValue(pivot_row, i), getDimension()));

        //updating stabilizer means we also need to update the X matrix, because its a VIEW on stabilizer (according to JW!!!!) 
        matrixX = newStabilizer->slice(0, getStabilizer()->getNumRows(), 
                                    numRows, getStabilizer()->getColumnsOfRow(0));

        newPhase->setValue(pivot_row, MathUtils::mod(inverseFactor * newPhase->getValue(pivot_row), getDimension()));

        for (int auxRow = 0; auxRow < numRows; auxRow++)
        {
            if (auxRow == pivot_row)
                continue;
            if (matrixX->getValue(auxRow, column) != 0)
                addRowsMultipleInPlace(newStabilizer, newPhase, pivot_row, auxRow, -(matrixX->getValue(auxRow, column)));  
        
            //updating stabilizer means we also need to update the X matrix, because its a VIEW on stabilizer (according to JW!!!!)     
            matrixX = newStabilizer->slice(0, getStabilizer()->getNumRows(), 
                                    numRows, getStabilizer()->getColumnsOfRow(0));
        }
        pivot_row++;
   }

   // Additionally bring the Z parts of the non-pivot columns to a form that is clearly separable
   // Z = T[:,:n]

   // for (int zerosRowIdx = pivot_row; zerosRowIdx < numRows; zerosRowIdx++)
   if(nonPivotColumns.size() != numRows - pivot_row)
        throw new invalid_argument("invalid size between nonPivotColumns and numRows - pivot_row");

    MatrixInt* matrixZ = newStabilizer->slice(0, newStabilizer->getNumRows(), 0, numRows);
    
   for (int i = 0; i < nonPivotColumns.size(); i++)
   {
        int column = nonPivotColumns[i];
        int zerosRowIdx = i + pivot_row;
        
        int rowIdx = matrixZ->indexMaxValueInCol(column, zerosRowIdx) + zerosRowIdx;

        if (matrixZ->getValue(rowIdx, column) == 0)
            throw invalid_argument("State::XtoRrefMod: T is not a valid stabilizer tableau.");

        newStabilizer->swapRows(zerosRowIdx, rowIdx);

        //updating stabilizer means we also need to update the Z matrix, because its a VIEW on stabilizer (according to JW!!!!)
        matrixZ = newStabilizer->slice(0, newStabilizer->getNumRows(), 0, numRows);

        newPhase->swapElements(zerosRowIdx, rowIdx);

        int inverseFactor = MathUtils::mod(MathUtils::power(matrixZ->getValue(zerosRowIdx, column), getDimension()-2), getDimension());

        for (int i = 0; i < newStabilizer->getColumnsOfRow(zerosRowIdx); i++)
            newStabilizer->setValue(zerosRowIdx, i, MathUtils::mod(inverseFactor * newStabilizer->getValue(zerosRowIdx, i), getDimension()));
            
        newPhase->setValue(zerosRowIdx, MathUtils::mod(inverseFactor * newPhase->getValue(zerosRowIdx), getDimension()));

        //updating stabilizer means we also need to update the Z matrix, because its a VIEW on stabilizer (according to JW!!!!)
        matrixZ = newStabilizer->slice(0, newStabilizer->getNumRows(), 0, numRows);

        for (int auxRow = 0; auxRow < numRows; auxRow++)
        {
            if (auxRow == zerosRowIdx)
                continue;
            if (matrixZ->getValue(auxRow, column) != 0)
                addRowsMultipleInPlace(newStabilizer, newPhase, zerosRowIdx, auxRow, -(matrixZ->getValue(auxRow, column)));             
               
            //updating stabilizer means we also need to update the Z matrix, because its a VIEW on stabilizer (according to JW!!!!)
            matrixZ = newStabilizer->slice(0, newStabilizer->getNumRows(), 0, numRows);                
        }
        
   }

   delete matrixX;
   delete matrixZ;

   return new State(newStabilizer, newPhase, getDimension());
}


State* State::reduceSeparableTableau(VectorInt* subset)
{
    // Return a reduced state on a subset of qudits, if possible.
    // Check if qudits in subset are separable from the resto of the state. If yes, return reduced tableau.
    State* newState = new State();
    newState = XtoRrefMod();
    int numRows = newState->getStabilizer()->getNumRows();

    VectorInt* rowsWithEntries = new VectorInt();

    for(int index = 0; index < subset->getSize(); index++)
    {
        if (subset->getValue(index) >= newState->getStabilizer()->getColumnsOfRow(0) || newState->getStabilizer()->getColumnsOfRow(0) <= subset->getValue(index)+numRows)
            throw invalid_argument("State::reduceSeparableTableau: subset is invalid.");
            
        // rowsWithEntries = rowsWithEntries | set(*np.nonzero(T[:,col])) | set(*np.nonzero(T[:,col+n]))
        VectorInt* firstList = newState->getStabilizer()->getFullColumn(subset->getValue(index))->nonzeroIndx();
        VectorInt* secondList = newState->getStabilizer()->getFullColumn(subset->getValue(index)+numRows)->nonzeroIndx();
        rowsWithEntries->concatenateAsSet(firstList);
        rowsWithEntries->concatenateAsSet(secondList);

        delete firstList;
        delete secondList;
    }

    //also here the newState must be reused according to JW
    if (rowsWithEntries->getSize() != subset->getSize())
        throw invalid_argument("State::reduceSeparableTableau: Tableau {T} is not separable with respect to {subset}.");

    for(int index = 0; index < rowsWithEntries->getSize(); index++)
    {
        int row = rowsWithEntries->getValue(index);
        for(int colIndex = 0; colIndex < numRows; colIndex++)
        {
            if(subset->contains(colIndex))
                continue;
            if(newState->getStabilizer()->getValue(row, colIndex) != 0 || newState->getStabilizer()->getValue(row, colIndex+numRows))
                throw invalid_argument("State::reduceSeparableTableau: Tableau {T} is not separable with respect to {subset}.");
        }
    }
    VectorInt* selectedCols = new VectorInt();

    //ATTENTION: here the order matters, as it gets used to construct final matrix using pushBack
    //---> if subset is 0,1, we need to first add 0,1 to selectedCols and after that then 3,4
    for (int idx = 0; idx < subset->getSize(); idx++) 
        selectedCols->pushBack(subset->getValue(idx));
    
    for (int idx = 0; idx < subset->getSize(); idx++) 
        selectedCols->pushBack(subset->getValue(idx)+numRows);

    return new State(newState->getStabilizer()->getSelectedElements(rowsWithEntries, selectedCols), newState->getPhase()->getSelectedElements(rowsWithEntries), getDimension());
}

void State::createPauliMatrices(MatrixInt* Z, MatrixInt* X, MatrixInt* id)
{
    VectorInt* oneZero = new VectorInt({1, 0});
    VectorInt* zeroMinusOne = new VectorInt({0, -1}); 
    VectorInt* zeroOne = new VectorInt({0, 1});

    Z->addRow(oneZero);
    Z->addRow(zeroMinusOne);

    X->addRow(zeroOne);
    X->addRow(oneZero);

    id->addRow(oneZero);
    id->addRow(zeroOne);
}

MatrixInt* State::kroneckerProduct(MatrixInt* a, MatrixInt* b)
{
    // Construct the identity matrix of n parites. KroneckerProduct(A,B) is the
    // tensor product of matrices A and B
    int rowa = a->getNumRows();
    int rowb = b->getNumRows();
    int cola = a->getColumnsOfRow(0);
    int colb = b->getColumnsOfRow(0);

    // Initialize result matrix 
    MatrixInt* result = new MatrixInt(rowa * rowb, cola * colb, 0);

    // i loops until rowa
    for (int i = 0; i < rowa; i++)
    {
        // j loops until cola
        for (int j = 0; j < cola; j++)
        {
            // k loops until rowb
            for (int k = 0; k < rowb; k++)
            {
                // l loops until colb
                for (int l = 0; l < colb; l++)
                {
                    // Each element of matrix A is multiplied by whole matrix B
                    // resp and stored as matrix C
                    result->setValue(i*rowb + k, j*colb + l, a->getValue(i, j) * b->getValue(k, l));
                }
            }
        }
    }
    return result;
}

MatrixComplex* State::kroneckerProduct(MatrixComplex* a, MatrixComplex b)
{
    // Construct the identity matrix of n parites. KroneckerProduct(A,B) is the
    // tensor product of matrices A and B
    int rowa = a->getNumRows();
    int rowb = b.getNumRows();
    int cola = a->getColumnsOfRow(0);
    int colb = b.getColumnsOfRow(0);

    // Initialize result matrix 
    MatrixComplex* result = new MatrixComplex(rowa * rowb, cola * colb, 0);

    // i loops until rowa
    for (int i = 0; i < rowa; i++)
    {
        // j loops until cola
        for (int j = 0; j < cola; j++)
        {
            // k loops until rowb
            for (int k = 0; k < rowb; k++)
            {
                // l loops until colb
                for (int l = 0; l < colb; l++)
                {
                    // Each element of matrix A is multiplied by whole matrix B
                    // resp and stored as matrix C
                    result->setValue(i*rowb + k, j*colb + l, a->getValue(i, j) * b.getValue(k, l));
                }
            }
        }
    }
    return result;
}

MatrixComplex* State::pureStateDensityMatrix()
{
    // Step 1: Define Pauli matrices
    MatrixInt* Z = new MatrixInt();
    MatrixInt* X = new MatrixInt();
    MatrixInt* id = new MatrixInt();
    createPauliMatrices(Z, X, id);

    // Step 2: Calculate density matrix
    int numRows = getStabilizer()->getNumRows();
    MatrixInt* newT = getStabilizer()->copy();
    MatrixInt* idN = id->copy();

    for (int i = 0; i < numRows-1; i++)
        idN = kroneckerProduct(idN, id);
    MatrixComplex* densityMatrix = new MatrixComplex(idN);
    complex<double> minusi(0,-1);
    
    for (int i = 0; i < numRows; i++)
    {
        MatrixInt* newZ = Z->matrixExp(newT->getValue(i,0));     // If the getValue is 1, then Z is considered set
        MatrixInt* newX = X->matrixExp(newT->getValue(i, numRows));    // If the getValue is 1, then X is considered set
        newX = newX->multiplyInt(MathUtils::power(-1, getPhase()->getValue(i)), false);
        MatrixComplex* gInt = new MatrixComplex(newZ->multiplyMatrices(newX));

        // If both X and Z are set, then we need to multiply with -i
        if (newT->getValue(i,0) == 1 && newT->getValue(i, numRows) == 1)
            gInt->multiplyByComplex(minusi, false);
        

        for (int j = 1; j < numRows; j++)
        {
            newZ = Z->matrixExp(newT->getValue(i, j));
            newX = X->matrixExp(newT->getValue(i, j+numRows));
            newZ = newZ->multiplyMatrices(newX);
            gInt = kroneckerProduct(gInt, MatrixComplex(newZ));

            // If both X and Z are set, then we need to multiply with -i
            if (newT->getValue(i, j) == 1 && newT->getValue(i, j+numRows) == 1)
                gInt->multiplyByComplex(minusi, false);
            
            delete newZ;
            delete newX;
        }
        gInt = gInt->sum(new MatrixComplex(idN), false); 
        densityMatrix = densityMatrix->multiplyMatrices(gInt);
        densityMatrix = densityMatrix->divisionByDouble(2, false);

        delete gInt;
    }

    delete Z;
    delete X;
    delete id;
    delete newT;
    delete idN;

    return densityMatrix;
}