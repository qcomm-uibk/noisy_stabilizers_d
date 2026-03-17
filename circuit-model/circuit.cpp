/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "circuit.h"
#include <string>

using namespace std;

Circuit::Circuit(int dimension, int numberOfQubits, Logger *logger)
{
    mOperations = new vector<Operation*>();
    mDimension = dimension;
    mNumberOfQubits = numberOfQubits;
    mLogger = logger;
}

Circuit::~Circuit()
{

}

void Circuit::AddOperation(Operation *op)
{
    mOperations->push_back(op);
}

vector<Operation*>* Circuit::GetOperations()
{
    return mOperations;
}

NoisyState* Circuit::Apply(NoisyState* input)
{
    NoisyState* n1 = input;

    mLogger->WriteLog("----- INPUT STATE FOR CIRCUIT -----");
    mLogger->WriteLog("Input state = ");
    n1->log(mLogger);

    int numberOperations = 0;
    
    for(Operation* op : *mOperations)
    {
        NoisyState* n2 = op->Apply(n1);

        delete n1;
        n1 = n2;

        mLogger->WriteLog("----- OPERATION -----");
        mLogger->WriteLog("Op = " + op->ToString());
        mLogger->WriteLog("State after Op = ");
        n1->log(mLogger);
        numberOperations++;
    }

    mLogger->WriteLog("----- OUTPUT STATE FOR CIRCUIT -----");
    mLogger->WriteLog("Output state = ");
    n1->log(mLogger);

    mLogger->WriteLog("#Operations was " + to_string(numberOperations));

    return n1;
}

int Circuit::getDimension()
{
    return mDimension;
}

int Circuit::getNumberOfQubits()
{
    return mNumberOfQubits;
}