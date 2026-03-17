/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef CLIFFORDOPERATIONSTESTS_H
#define CLIFFORDOPERATIONSTESTS_H

#pragma once

#include <string>
#include "../logging/logger.h"

using namespace std;

class CliffordOperationsTests
{
public:
    CliffordOperationsTests(Logger *logger);
    ~CliffordOperationsTests();

    void runHadamardTest();
    void runNoisyHadamardTest();
    
    void runCzTest();
    void runPhaseGateTest();

private:
    Logger *mLogger;

    void writeResult(bool result, string message, Logger *logger);
};

#endif