/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef OPERATION_H
#define OPERATION_H

#pragma once

#include "noisyState.h"

class Operation
{
public:
    Operation();
    ~Operation();

    virtual NoisyState* Apply(NoisyState *state) = 0;
    virtual string ToString() = 0;

private:

};

#endif