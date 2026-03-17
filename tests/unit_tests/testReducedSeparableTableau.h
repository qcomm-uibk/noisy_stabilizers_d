/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef TESTREDUCEDSEPARABLETABLEAU_H
#define TESTREDUCEDSEPARABLETABLEAU_H

#pragma once

// This is an example that generates a random graph state that has a guaranteed
// disconnected subset and then applies some local Cliffords (not general).
// Then, the stabilizer generators are shuffled and mixed to obfuscate the
// disconnected nature. Our algorithm should be able to recover the same
// reduced state after the obfuscation.

#include <string>
#include "../../utils/graph.h"
#include "../../logging/logger.h"


using namespace std;

class TestReducedSeparableTableau
{
public:
    TestReducedSeparableTableau(Logger *logger);
    ~TestReducedSeparableTableau();

    void runTest();

private:
    Graph generateRandomGraph(long nodes, double p);

    Logger *mLogger;

    void writeResult(bool result, string message, Logger *logger);
};


#endif