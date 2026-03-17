/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef CIRCUITPARSERQUBITS_H
#define CIRCUITPARSERQUBITS_H

#pragma once

#include <string>
#include <tuple>

#include "../../logging/logger.h"
#include "../circuit.h"
#include "../../includes/json.hpp"

using namespace std;
using json = nlohmann::json;

class CircuitParserQubits
{
public:
    CircuitParserQubits(Logger* logger);
    ~CircuitParserQubits();

    Circuit* Parse(string fileName);
private:
    Logger *mLogger;
    tuple<NoiseMap*,string> getChannel(json noise, int qubits, int dimension);
    
    const string delimiter = ",";
};

#endif