/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#include "cliffordOperationsTests.h"
#include "../circuit-model/circuit.h"
#include "../circuit-model/jsonParser/circuitParserQubits.h"
#include "../circuit-model/noisyState.h"
#include "../circuit-model/stateStreams/csvStateStream.h"
#include "../circuit-model/cliffordOperations/hadamard.h"
#include "../circuit-model/cliffordOperations/phaseGate.h"
#include "../circuit-model/cliffordOperations/czGate.h"
#include "../logging/loggerCout.h"
#include "../logging/nullLogger.h"

CliffordOperationsTests::CliffordOperationsTests(Logger *logger)
{
    mLogger = logger;
}

CliffordOperationsTests::~CliffordOperationsTests()
{
}

void CliffordOperationsTests::runHadamardTest()
{
    mLogger->WriteLog("****** HADAMARD TESTS");

    CsvStateStream stream(nullptr);


    NoisyState* input = stream.read("test_classes/test_data/hadamard/bellstate.csv", 
        "test_classes/test_data/hadamard/bellstate-phase.csv", 
        vector<tuple<string, string>>(), 
        2);

    NoisyState* expected1 = stream.read("test_classes/test_data/hadamard/hadamard-1-output-twoqubit.csv", 
        "test_classes/test_data/hadamard/bellstate-phase.csv", 
        vector<tuple<string, string>>(), 
        2);

     NoisyState* expected2 = stream.read("test_classes/test_data/hadamard/hadamard-2-output-twoqubit.csv", 
        "test_classes/test_data/hadamard/bellstate-phase.csv", 
        vector<tuple<string, string>>(), 
        2);
    
    CircuitParserQubits parser(new NullLogger());
    Circuit *circuit1 = parser.Parse("test_classes/test_data/hadamard/circuit-1.json");

    //Hadamard hQubit1(2, 0);
    Hadamard hQubit2(1, vector<NoiseMap*>());

    NoisyState* n1 = circuit1->Apply(input);
    NoisyState* n2 = hQubit2.Apply(input);

    NoisyState* n3 = circuit1->Apply(n1);
    NoisyState* n4 = hQubit2.Apply(n2);

    writeResult(n1->isEqual(expected1), "H_0 * b", mLogger);
    writeResult(n2->isEqual(expected2), "H_1 * b", mLogger);
    writeResult(n3->isEqual(input), "H_0 * H_0 * b = id * b", mLogger);
    writeResult(n4->isEqual(input), "H_1 * H_1 * b = id * b", mLogger);

    mLogger->WriteLog("****** COMPLETED");
    mLogger->WriteLog("");
}

void CliffordOperationsTests::runNoisyHadamardTest()
{
    mLogger->WriteLog("****** NOISY HADAMARD TESTS");

    CsvStateStream stream(nullptr);

    // NoisyState input = stream.read("test_classes/test_data/hadamard/bellstate.csv", 
    //     "test_classes/test_data/hadamard/bellstate-phase.csv", 
    //     vector<tuple<string, string>>{
    //         {
    //             "test_classes/test_data/noisemaps/noisemap-n1-state.csv",
    //             "test_classes/test_data/noisemaps/noisemap-n1-weights.csv"
    //         }
    //     }, 
    //     2);

    NoisyState* input = stream.read("test_classes/test_data/hadamard/bellstate.csv", 
        "test_classes/test_data/hadamard/bellstate-phase.csv", 
        vector<tuple<string, string>>(), 
        2);

    CircuitParserQubits parser(new LoggerCout());
    Circuit *circuit1 = parser.Parse("test_classes/test_data/hadamard/circuit-2.json");
    
    NoisyState* n2 = circuit1->Apply(input);
    
    // n2.log(mLogger);
    // NoisyState n3 = hQubit1.Apply(n1);
    // NoisyState n4 = hQubit2.Apply(n2);

    // writeResult(n1.isEqual(expected1), "H_0 * b", mLogger);
    // writeResult(n2.isEqual(expected2), "H_1 * b", mLogger);
    // writeResult(n3.isEqual(input), "H_0 * H_0 * b = id * b", mLogger);
    // writeResult(n4.isEqual(input), "H_1 * H_1 * b = id * b", mLogger);

    mLogger->WriteLog("****** COMPLETED");
    mLogger->WriteLog("");
}

void CliffordOperationsTests::runCzTest()
{
    mLogger->WriteLog("****** CZ TESTS (CZ_{0 ->1})");

    //input1: 
    // 1: X_s --> X_s Z_t
    // 2: Z_s --> Z_s
    //input2: 
    // 1: X_t --> X_t Z_s
    // 2: Z_t --> Z_t

    CsvStateStream stream(nullptr);

    NoisyState* input1 = stream.read("test_classes/test_data/czgate/input1.csv", 
        "test_classes/test_data/czgate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);
    
    NoisyState* input2 = stream.read("test_classes/test_data/czgate/input2.csv", 
        "test_classes/test_data/czgate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);

    NoisyState* expected1 = stream.read("test_classes/test_data/czgate/result1.csv", 
        "test_classes/test_data/czgate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);

    NoisyState* expected2 = stream.read("test_classes/test_data/czgate/result2.csv", 
        "test_classes/test_data/czgate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);

    CzGate cz(0, 1, vector<NoiseMap*>());
    
    NoisyState* n1 = cz.Apply(input1);
    NoisyState* n2 = cz.Apply(input2);

    writeResult(n1->isEqual(expected1), "CZ_{0 ->1}: X_s, Z_s ", mLogger);
    writeResult(n2->isEqual(expected2), "CZ_{0 ->1}: X_t, Z_t", mLogger);

    mLogger->WriteLog("****** COMPLETED");
    mLogger->WriteLog("");
}

void CliffordOperationsTests::runPhaseGateTest()
{
    mLogger->WriteLog("****** PHASE GATE TESTS");

    CsvStateStream stream(nullptr);

    NoisyState* input = stream.read("test_classes/test_data/phasegate/state.csv", 
        "test_classes/test_data/phasegate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);

    NoisyState* expected1 = stream.read("test_classes/test_data/phasegate/result1.csv", 
        "test_classes/test_data/phasegate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);
    
    NoisyState* expected2 = stream.read("test_classes/test_data/phasegate/result2.csv", 
        "test_classes/test_data/phasegate/0phase.csv", 
        vector<tuple<string, string>>(), 
        2);
    
    PhaseGate phase1(0, vector<NoiseMap*>());
    PhaseGate phase2(1, vector<NoiseMap*>());
    
    NoisyState* n1 = phase1.Apply(input);
    NoisyState* n2 = phase2.Apply(input);

    NoisyState* n3 = phase1.Apply(n1);
    NoisyState* n4 = phase2.Apply(n2);

    writeResult(n1->isEqual(expected1), "S_0 * b", mLogger);
    writeResult(n2->isEqual(expected2), "S_1 * b", mLogger);
    
    mLogger->WriteLog("****** COMPLETED");
    mLogger->WriteLog("");   
}

void CliffordOperationsTests::writeResult(bool result, string message, Logger *logger)
{
    if(result)
        logger->WriteLog("OK: " + message);
    else
        logger->WriteLog("ERROR: " + message);
}