/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "circuitParserQubits.h"
#include "../../includes/json.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "../circuit.h"
#include "../operation.h"

#include "../cliffordOperations/czGate.h"
#include "../cliffordOperations/hadamard.h"
#include "../cliffordOperations/phaseGate.h"
#include "../cliffordOperations/multiplicationGate.h"
#include "../cliffordOperations/zGate.h"
#include "../cliffordOperations/quantumChannel.h"

#include "../measurements/measureZ.h"
#include "../../utils/parseUtils.h"

using json = nlohmann::json;
using namespace std;

CircuitParserQubits::CircuitParserQubits(Logger *logger)
{
    mLogger = logger;
}

CircuitParserQubits::~CircuitParserQubits()
{
}

Circuit* CircuitParserQubits::Parse(string fileName)
{
    std::ifstream f(fileName);
    json data = json::parse(f);

    auto input = data["input"];
    auto circuit = data["input"]["circuit"];

    mLogger->WriteLog("Read circuit from file with name " + fileName);
    
    int dimension = input["dimension"].template get<int>();
    int qubits = input["systems"].template get<int>();

    mLogger->WriteLog("Dimensionality of circuit is " + to_string(dimension));
    mLogger->WriteLog("Number of systems of circuit is " + to_string(qubits));

    Circuit* collectedCircuit = new Circuit(dimension, qubits, mLogger);
    
    for (size_t i = 0; i < circuit.size(); ++i) {
        const auto operation = circuit[i];
        mLogger->WriteLog("Found operation " + to_string(operation) + ", attempting to parse it ...");
        
        vector<NoiseMap*> noiseMaps;
        vector<NoiseMap*> emptyNoiseMaps;

        //check for noise
        if(operation.find("noise") != operation.end())
        {
            //{"channel":"depolarizing","lambda":0.989,"qubits":[0]
            auto noiseCollection = operation["noise"];
            for (int j = 0; j < noiseCollection.size(); j++)
            {
                auto noise = noiseCollection[j];

                if (noise.find("channel") != noise.end())
                {
                    auto noiseMapTuple = getChannel(noise, qubits, dimension);
                    noiseMaps.push_back(std::get<0>(noiseMapTuple));
                }
            }
        }

        if(operation.find("channel") != operation.end())
        {
            auto noiseMapTuple = getChannel(operation, qubits, dimension);

            collectedCircuit->AddOperation(new QuantumChannel(std::get<0>(noiseMapTuple), std::get<1>(noiseMapTuple)));
        }

        if(operation.find("measurement") != operation.end())
        {
            auto measurement = operation["measurement"].template get<std::string>();
            if(measurement.compare("z") == 0)
            {
                auto autoIndexArray = operation["measure"];
                int index = (int)autoIndexArray[0];

                if(operation.find("outcome") !=  operation.end())
                {
                    int outcome = (int)operation["outcome"];
                    mLogger->WriteLog("Measurement outcome chosen to be: " + to_string(outcome));

                    collectedCircuit->AddOperation(new MeasureZ(index, false, noiseMaps, outcome, dimension));
                }
                else    
                    collectedCircuit->AddOperation(new MeasureZ(index, false, noiseMaps));
            }
            if(measurement.compare("bell") == 0)
            {
                auto autoIndexArray = operation["measure"];
                int index1 = (int)autoIndexArray[0];
                int index2 = (int)autoIndexArray[1];


                //bell-measurement = CNOT_i1->i2 * H_i1 * Zm_i1 * Zm_i2

                //CNOT
                collectedCircuit->AddOperation(new Hadamard(index2, noiseMaps));
                collectedCircuit->AddOperation(new CzGate(index1, index2, emptyNoiseMaps));
                collectedCircuit->AddOperation(new Hadamard(index2, emptyNoiseMaps));

                collectedCircuit->AddOperation(new Hadamard(index1, emptyNoiseMaps));

                if(operation.find("outcome") !=  operation.end())
                {
                    auto outcome = (int)operation["outcome"];

                    int outcome1 = 0;
                    int outcome2 = 0;

                    switch(outcome)
                    {
                        case 1: 
                            outcome1 = 0;
                            outcome2 = 1;
                        break;

                        case 2:
                            outcome1 = 1;
                            outcome2 = 0;
                        break;

                        case 3:
                            outcome1 = 1;
                            outcome2 = 1;
                        break;
                    }

                    mLogger->WriteLog("Measurement outcome chosen to be: " + to_string(outcome1) + " and " + to_string(outcome2)) ;

                    collectedCircuit->AddOperation(new MeasureZ(index1, false, emptyNoiseMaps, outcome1, dimension));
                    collectedCircuit->AddOperation(new MeasureZ(index2, false, emptyNoiseMaps, outcome2, dimension));
                }
                else    
                {
                    collectedCircuit->AddOperation(new MeasureZ(index1, false, emptyNoiseMaps));
                    collectedCircuit->AddOperation(new MeasureZ(index2, false, emptyNoiseMaps));
                }
            }
        }
        if(operation.find("gate") != operation.end())
        {
            auto gate = operation["gate"].template get<std::string>();
            
            if(gate.compare("multiplication") == 0)
            {
                int factor = (int)operation["factor"];

                auto autoIndexArray = operation["target"];
                int index = (int)autoIndexArray[0];
                    
                collectedCircuit->AddOperation(new MultiplicationGate(index, factor));
            }
            if(gate.compare("zgate") == 0)
            {
                auto autoIndexArray = operation["target"];
                int index = (int)autoIndexArray[0];
                    
                collectedCircuit->AddOperation(new ZGate(index));
            }
            if(gate.compare("h") == 0)
            {
                auto autoIndexArray = operation["target"];
                int index = (int)autoIndexArray[0];
                    
                collectedCircuit->AddOperation(new Hadamard(index, noiseMaps));
            }
            if(gate.compare("cnot") == 0)
            {
                int source = (int)operation["control"][0];
                int target = (int)operation["target"][0];

                //CNOT_s->t = H_t * CZ_s->t * H_t 
                collectedCircuit->AddOperation(new Hadamard(target, noiseMaps));
                collectedCircuit->AddOperation(new CzGate(source, target, emptyNoiseMaps));
                collectedCircuit->AddOperation(new Hadamard(target, emptyNoiseMaps));
            }
            if(gate.compare("cz") == 0)
            {
                int source = (int)operation["control"][0];
                int target = (int)operation["target"][0];

                collectedCircuit->AddOperation(new CzGate(source, target, noiseMaps));
            }
            if(gate.compare("s") == 0)
            {
                int target = (int)operation["target"][0];
                collectedCircuit->AddOperation(new PhaseGate(target, noiseMaps)); 
            }
            if(gate.compare("x") == 0)
            {
                int target = (int)operation["target"][0];
                
                //X = H*Z*H = H*S*S*H
                collectedCircuit->AddOperation(new Hadamard(target, noiseMaps));

                collectedCircuit->AddOperation(new PhaseGate(target, emptyNoiseMaps)); 
                collectedCircuit->AddOperation(new PhaseGate(target, emptyNoiseMaps));  

                collectedCircuit->AddOperation(new Hadamard(target, emptyNoiseMaps));
            }
            if(gate.compare("y") == 0)
            {
                int target = (int)operation["target"][0];
                //cout << "Y gate found for index " << index << std::endl;                
                
            }
            if(gate.compare("z") == 0)
            {
                int target = (int)operation["target"][0];
                
                //Z = S^2
                collectedCircuit->AddOperation(new PhaseGate(target, noiseMaps)); 
                collectedCircuit->AddOperation(new PhaseGate(target, emptyNoiseMaps));    
            }
        }
    }

    mLogger->WriteLog("Full circuit has been parsed, collected operations below:");

    for(Operation* op : *collectedCircuit->GetOperations())
        mLogger->WriteLog(op->ToString());
    
    return collectedCircuit;
}

tuple<NoiseMap*,string> CircuitParserQubits::getChannel(json noise, int qubits, int dimension)
{
    auto noiseType = noise["channel"].template get<std::string>();
    if (noiseType.compare("arbitrary") == 0)
    {
        //{
        //    "channel":"arbitrary",
        //    "operators":["0,1,1,0","1,0,0,1"],
        //    "weights":["0.5","0.5"]
        //}
        auto weightsArray = noise["weights"];

        vector<double> weights(weightsArray.size());
        for(int i=0; i<weightsArray.size(); i++)        
            weights[i] = (double)weightsArray[i];        

        auto operatorsArray = noise["operators"];

        vector<vector<int>> operators;
        for(int i=0; i<operatorsArray.size(); i++)
        {
            string opString = operatorsArray[i].template get<std::string>();
            vector<int> op = ParseUtils::split(opString, this->delimiter);

            if(op.size() == 2*qubits)
            {
                operators.push_back(op);
            }
        }

        MatrixInt* matrix = new MatrixInt(operators);
        VectorDouble* weightsVector = new VectorDouble(weights);

        mLogger->WriteLog("Found arbitrary noise with " + to_string(weights.size()) + " maps");
        NoiseMap* noiseMap = new NoiseMap(matrix, weightsVector);
        return make_tuple(noiseMap, "Arbitrary noise map with " + to_string(weights.size()) + " maps");
    }
    // depolarizing channel
    if (noiseType.compare("depolarizing") == 0)
    {
        // depolarizing channel --> get parameters
        double lambda = (double)noise["lambda"];

        auto autoIndexArray = noise["qubits"];
        int index = (int)autoIndexArray[0];

        vector<vector<int>> operators;
        for (int op = 0; op < 4; op++)
        {
            vector<int> currentOperator(2 * qubits);
            for (int qu = 0; qu < qubits; qu++)
            {
                currentOperator[qu] = 0;
                currentOperator[qu + qubits] = 0;
                if (qu == index)
                {
                    switch (op)
                    {
                    // Z operator
                    case 1:
                        currentOperator[qu] = 1;
                        break;
                    // X operator
                    case 2:
                        currentOperator[qu + qubits] = 1;
                        break;
                    // Y operator
                    case 3:
                        currentOperator[qu] = 1;
                        currentOperator[qu + qubits] = 1;
                        break;
                    }
                }
            }
            operators.push_back(currentOperator);
        }

        vector<double> weights(4);
        weights[0] = 1 - 3 * lambda / 4;
        weights[1] = lambda / 4;
        weights[2] = lambda / 4;
        weights[3] = lambda / 4;

        MatrixInt* matrix = new MatrixInt(operators);
        VectorDouble* weightsVector = new VectorDouble(weights);

        mLogger->WriteLog("Found depolarizing noise on index " + to_string(index) + " with lambda=" + to_string(lambda));
        NoiseMap* noiseMap = new NoiseMap(matrix, weightsVector);
        return make_tuple(noiseMap, "Depolarizing channel on index " + to_string(index) + " with lambda=" +  to_string(lambda));
    }

    if (noiseType.compare("bitflip") == 0)
    {
        double p = (double)noise["probability"];

        auto autoIndexArray = noise["qubits"];
        int index = (int)autoIndexArray[0];

        vector<vector<int>> operators;
        for (int op = 0; op < 2; op++)
        {
            vector<int> currentOperator(2 * qubits);
            for (int qu = 0; qu < qubits; qu++)
            {
                currentOperator[qu] = 0;
                currentOperator[qu + qubits] = 0;
                if (qu == index)
                {
                    switch (op)
                    {
                    // X operator
                    case 1:
                        currentOperator[qu + qubits] = 1;
                        break;
                    }
                }
            }
            operators.push_back(currentOperator);
        }

        vector<double> weights(2);
        weights[0] = 1 - p;
        weights[1] = p;

        MatrixInt *matrix = new MatrixInt(operators);
        VectorDouble* weightsVector = new VectorDouble(weights);

        mLogger->WriteLog("Found bitflip noise on index " + to_string(index) + " with p=" + to_string(p));
        NoiseMap* noiseMap = new NoiseMap(matrix, weightsVector);
        return make_tuple(noiseMap, "Bitflip channel on index " + to_string(index) + "with p=" +  to_string(p));
    }

    if (noiseType.compare("phaseflip") == 0)
    {
        //   \"probability\": 0.5,
        //   \"qubits\": [1]
        double p = (double)noise["probability"];

        auto autoIndexArray = noise["qubits"];
        int index = (int)autoIndexArray[0];

        vector<vector<int>> operators;
        for (int op = 0; op < 2; op++)
        {
            vector<int> currentOperator(2 * qubits);
            for (int qu = 0; qu < qubits; qu++)
            {
                currentOperator[qu] = 0;
                currentOperator[qu + qubits] = 0;
                if (qu == index)
                {
                    switch (op)
                    {
                    // Z operator
                    case 1:
                        currentOperator[qu] = 1;
                        break;
                    }
                }
            }
            operators.push_back(currentOperator);
        }

        vector<double> weights(2);
        weights[0] = 1 - p;
        weights[1] = p;

        MatrixInt* matrix = new MatrixInt(operators);
        VectorDouble* weightsVector = new VectorDouble(weights);

        mLogger->WriteLog("Found phaseflip noise on index " + to_string(index) + " with p=" + to_string(p));
        NoiseMap* noiseMap = new NoiseMap(matrix, weightsVector);
        return make_tuple(noiseMap, "Phaseflip channel on index " + to_string(index) + " with p=" +  to_string(p));
    }

    NoiseMap* noiseMap = new NoiseMap(new MatrixInt(), new VectorDouble());
    return make_tuple(noiseMap, "");
}