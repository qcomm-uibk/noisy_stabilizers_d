#include "includes/json.hpp"

#include <iostream>
#include <vector>
#include <string>

#include "circuit-model/jsonParser/circuitParserQubits.h"
#include "circuit-model/stateStreams/csvStateStream.h"
#include "circuit-model/noisyState.h"

#include "logging/logger.h"
#include "logging/loggerCout.h"
#include "logging/nullLogger.h"
#include "logging/loggerFile.h"

#include "utils/stopWatch.h"
#include "utils/mathUtils.h"

#include "circuit-model/cliffordOperations/hadamard.h"
#include "test_classes/cliffordOperationsTests.h"
#include "tests/unit_tests/testReducedSeparableTableau.h"

using namespace std;
using json = nlohmann::json;

vector<string> parseInputStateFile(ifstream &file);
vector<vector<string>> parseNoiseMapsFile(ifstream &file);
vector<string> parseTestsFile(ifstream &file);

NoisyState* runCircuit(string inputFilesState, string inputFilesNoiseMaps, string circuitJson, Logger* logger, string logParameter);
NoisyState* readNoisyState(string inputFilesState, string inputFilesNoiseMaps, int dimension, Logger* logger);
vector<string> split(string s, string delimiter);

//./nsfd input_examples/inputstatespec.csv input_examples/noisemapsspec.csv input_examples/circuit.json out/ none reduced 1,2
// OPTIONS for logging: none/console/file
// OPTIONS for add-on: none/density/reduced --> reduced requires also a list of integers for subsystem
// IMPORTANT: when using reduced, the subfolder "out/reduced" MUST exist!

//./nsfd network_node_examples/inputstatespec.csv network_node_examples/noisemapsspec.csv network_node_examples/output-circuit.json out/ console
int main(int argc, char *argv[])
{
    try
    {
        if(argc < 7)
        {
            if(argc == 2)
            {   
                //generate state file in input_examples folder where second argument corresponds to the number of qubits
                string numberOfQubitsString = argv[1];
                int numberOfQubits = stoi(numberOfQubitsString);

                vector<vector<int>> stabilizer;
                vector<int> phase;
                for(int i=0; i<numberOfQubits; i++)
                { 
                    vector<int> row;
                    for(int j=0; j<numberOfQubits; j++)
                    {
                        row.push_back(rand() % 2);
                        row.push_back(rand() % 2);
                    }

                    stabilizer.push_back(row);
                    phase.push_back(rand() % 2);
                }

                CsvStateStream stream(nullptr);
                ofstream stabilizerFs("stabilizer.csv");
                ofstream phaseFs("phase.csv");
            
                stream.writeStateFile(stabilizerFs, new MatrixInt(stabilizer));
                stream.writePhaseFile(phaseFs, new VectorInt(phase));

                return 0;
            }

            ifstream testsFile("tests/tests.csv");
            vector<string> testDirectories = parseTestsFile(testsFile);
            Logger* logger = new NullLogger();

            vector<string> failedTests;
            for(int i=0; i<testDirectories.size(); i++)
            {
                string directory = testDirectories[i];
                string inputStateSpec = directory + "/input/inputstatespec.csv";
                string inputNoiseMapSpec = directory + "/input/noisemapsspec.csv";
                string circuit = directory + "/input/circuit.json";
                string testspec = directory + "/input/testspec.json";
              
                bool isOk = true;
                ifstream testspecFile(testspec);

                if(testspecFile.good())
                {
                    json data = json::parse(testspecFile);

                    cout << "+++++ Running TESTs in directory " << directory << " +++++" << std::endl;

                    NoisyState* n = runCircuit(inputStateSpec, inputNoiseMapSpec, circuit, logger, "none");

                    auto input = data["input"];
                    for (size_t i = 0; i < input.size(); ++i) {
                        const auto test = input[i];

                        vector<int> reduce;
                        if(test.find("reduce") != test.end())
                        {
                            auto autoIndexArray = test["reduce"];
                            for(int j = 0; j<autoIndexArray.size(); j++)
                                reduce.push_back((int)autoIndexArray[j]);
                        }

                        string compareType;
                        if(test.find("compare") != test.end())
                        {
                            compareType = test["compare"].template get<std::string>();
                        }

                        bool containsFidelity = false;
                        double expectedFidelity = 0.0;                        
                        if(test.find("fidelity") != test.end())
                        {
                            auto autoFidelityArray = test["fidelity"];
                            expectedFidelity = (double)autoFidelityArray[0];
                            containsFidelity = true;
                        }

                        string resultMatrixFileName;
                        if(test.find("matrix_file") != test.end())
                        {
                            resultMatrixFileName = test["matrix_file"].template get<std::string>();
                        }

                        cout << "*** Running TEST " << to_string(i+1) << std::endl;

                        //if reduce is set, we need to reduce state before computing what we are told
                        NoisyState* resultWithPotentialReduce = n->copy();
                        if(reduce.size() > 0)
                        {
                            string subsystems;
                            for(int k=0; k<reduce.size(); k++)
                            {
                                
                                subsystems = subsystems + to_string(reduce.at(k));
                                if(k != reduce.size() - 1)
                                    subsystems = subsystems + ",";
                            }

                            cout << "Reduced output state to systems (" << subsystems << ")" << std::endl;
                            resultWithPotentialReduce = resultWithPotentialReduce->reducedNoisyState(new VectorInt(reduce));
                        }

                        if(compareType.compare("stabilizers") == 0)
                        {
                            //run the normal tests only if "stabilizers" is specified as compare
                            string outputStateSpec = directory + "/output/inputstatespec.csv";
                            string outputNoiseMapSpec = directory + "/output/noisemapsspec.csv";

                            NoisyState* outputState = readNoisyState(outputStateSpec, outputNoiseMapSpec, n->getDimension(), logger);
                            bool isTestOk = resultWithPotentialReduce->isEqual(outputState);

                            if(!isTestOk)
                                cout << "TEST Stabilizers: FAILED" << std::endl;
                            else
                                cout << "TEST Stabilizers: OK" << std::endl;

                            isOk &= isTestOk;
                            continue;
                        }
                        
                        MatrixComplex* resultMatrix;
                        if(compareType.compare("pure_state") == 0)
                        {
                            resultMatrix = resultWithPotentialReduce->getState()->pureStateDensityMatrix();
                        }
                        if(compareType.compare("mixed_state") == 0)
                        {
                            resultMatrix = resultWithPotentialReduce->calculateMixedStateDensityMatrix(logger);
                        }

                        ifstream resultMatrixFile(directory + "/output/" + resultMatrixFileName);
                        CsvStateStream stateStream(nullptr);
                        MatrixComplex* expectedResultMatrix = stateStream.parseMatrixComplexFile(resultMatrixFile);

                        bool isEqual = resultMatrix->isEqual(expectedResultMatrix);

                        if(!isEqual)
                        {
                            cout << "TEST Matrix (" << compareType << "): FAILED" << std::endl;
                            isOk &= false;
                        }
                        else
                        {
                            cout << "TEST Matrix (" << compareType << "): OK" << std::endl;
                        }

                        if(containsFidelity)
                        {                            
                            double f = resultWithPotentialReduce->fidelity(logger);
                            double difference = fabs(f - expectedFidelity);

                            if(difference > MathUtils::getTolerance())
                            {
                                cout << "TEST Fidelity: FAILED" << std::endl;
                                isOk &= false;    
                            } 
                            else
                            {
                                cout << "TEST Fidelity: OK" << std::endl;
                            }                           
                        }

                        delete resultWithPotentialReduce;
                    }

                    string result = "OK";
                    if(!isOk)
                    {
                        result = "--- FAILED ---";
                        failedTests.push_back(directory);
                    }

                    cout << "TEST results: " << result << std::endl;
                    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                }

                

                //TEMP: verify that we can create and write density matrix to file
                // if(i == testDirectories.size() - 1)
                // {   
                //     MatrixComplex* densityMatrix = n->getState()->pureStateDensityMatrix();
                //     (new LoggerCout())->WriteLog("Pure state:");
                //     densityMatrix->logMatrix(new LoggerCout());

                //     MatrixComplex* mixedState = n->calculateMixedStateDensityMatrix(new LoggerCout());
                //     (new LoggerCout())->WriteLog("Mixed state:");
                //     mixedState->logMatrix(new LoggerCout());

                //     (new LoggerCout())->WriteLog("Fidelity:");
                //     (new LoggerCout())->WriteLog(to_string(n->fidelity(new NullLogger())));

                //     // JW: Here is something off, but don't know exactly what: when using the circuit below, together with the reduce functions
                //     // I get when starting with two bell-states in tensor product after the circuit and by reducing to qubit 0 only the following
                //     // state: 
                //     // (1,0)
                //     // (0,0)
                //     // I am pretty sure we should use for these cases the "reduce_noisy_state_post_meas", no?
                    
                //     // "circuit":[ 
                //     //     {"channel":"bitflip","probability":0.1,"qubits":[0]},
                //     //     {"outcome":1,"measurement":"z","measure":[0]}            
                //     // ]

                //     vector<int> subset;
                //     subset.push_back(0);
                //     //subset.push_back(3);
                //     // subset.push_back(3);

                //     NoisyState* reduced = n->reducedNoisyState(new VectorInt(subset));
                //     (new LoggerCout())->WriteLog("Reduced noisy state (stabs):");
                //     reduced->log(new LoggerCout());

                //     (new LoggerCout())->WriteLog("Reduced noisy state (density):");
                //     MatrixComplex* mixedReduced = reduced->calculateMixedStateDensityMatrix(new LoggerCout());
                //     mixedReduced->logMatrix(new LoggerCout());   
                // }
            }

            for(int i=0; i<failedTests.size(); i++)
            {
                cout << "*** FAILED TEST in directory " << failedTests[i] << std::endl;
            }
            
            cout << "*** Running graph test " << std::endl;
            TestReducedSeparableTableau* test = new TestReducedSeparableTableau(new LoggerCout());
            test->runTest();

            //TODO Alex: Create matrix and check if remove column etc works                
            // void deleteValue(int row, int col);
            // void removeRow(int row);
            // void removeColumn(int col);
        }
        else
        {
            string inputFilesState = argv[1];
            string inputFilesNoiseMaps = argv[2];
            string circuitJson = argv[3];
            string outputDir = argv[4];
            string logParameter = argv[5];
            string densityParameter = argv[6];

            Logger *logger = nullptr;
            if(logParameter.compare("console") == 0)
            {
                logger = new LoggerCout();
            }
            else if(logParameter.compare("none") == 0)
            {
                logger = new NullLogger();
            }
            else if(logParameter.compare("file") == 0)
            {
                logger = new LoggerFile();
            }

            logger->WriteLog("NSF-d launched with the following arguments:");  
            for(int i=0; i<argc; i++){
                logger->WriteLog(argv[i]);            
            }

            bool densityMatrix = false;
            if(densityParameter.compare("density") == 0)
            {
                logger->WriteLog("DENSITY OPTION set");
                densityMatrix = true;
            }

            bool reduced = false;
            if(densityParameter.compare("reduced") == 0)
            {
                logger->WriteLog("REDUCED OPTION set");
                reduced = true;
            }
        
            NoisyState* outputState = runCircuit(inputFilesState, inputFilesNoiseMaps, circuitJson, logger, logParameter);
            string stateOutputStabilizer = outputDir + "stabilizers.csv";
            string stateOutputPhase = outputDir + "phase.csv";
            string pureStateDensityMatrixFile = outputDir + "pureStateDensityMatrix.csv";
            string mixedStateDensityMatrixFile = outputDir + "mixedStateDensityMatrix.csv";

            vector<NoiseMap*> outputNoiseMaps = outputState->getNoiseMaps();
            vector<tuple<string, string>> noiseOutputFiles;

            int numNoiseMaps = outputNoiseMaps.size();

            for(int i=0; i<numNoiseMaps; i++)
            {
                string noiseOperatorFile = outputDir + "noisemap"+to_string(i)+"-operators.csv";
                string weightsFile = outputDir + "noisemap"+to_string(i)+"-weights.csv";

                noiseOutputFiles.push_back(make_tuple(noiseOperatorFile, weightsFile));
            }

            CsvStateStream stateStream(nullptr);
            stateStream.write(outputState, stateOutputStabilizer, stateOutputPhase, noiseOutputFiles, outputState->getDimension());

            if(densityMatrix)
            {
                logger->WriteLog("Calculating density matrix ... ");

                MatrixComplex* pureStateDensityMatrix = outputState->getState()->pureStateDensityMatrix();
                logger->WriteLog("Pure state density matrix:");                
                pureStateDensityMatrix->logMatrix(logger);

                MatrixComplex* mixedStateDensityMatrix = outputState->calculateMixedStateDensityMatrix(logger);
                logger->WriteLog("Mixed state density matrix:");                
                mixedStateDensityMatrix->logMatrix(logger);

                logger->WriteLog("Fidelity:");   
                logger->WriteLog(to_string(outputState->fidelity(logger)));

                ofstream mixedStateDensityMatrixFileHandle(mixedStateDensityMatrixFile);
                stateStream.writeMatrixComplexFile(mixedStateDensityMatrixFileHandle, mixedStateDensityMatrix);

                ofstream pureStateDensityMatrixFileHandle(pureStateDensityMatrixFile);
                stateStream.writeMatrixComplexFile(pureStateDensityMatrixFileHandle, pureStateDensityMatrix);
            }

            if(reduced)
            {                
                logger->WriteLog("Calculating reduced, separable matrix ... ");
                string subSystemsString = argv[7];
                vector<string> subSystemsArrayString = split(subSystemsString, ",");

                vector<int> subset;
                for(int i=0; i<subSystemsArrayString.size(); i++)
                    subset.push_back(stoi(subSystemsArrayString[i]));

                logger->WriteLog("Subset indices: " + subSystemsString);      

                NoisyState* reduced = outputState->reducedNoisyState(new VectorInt(subset));               
                MatrixComplex* mixedReduced = reduced->calculateMixedStateDensityMatrix(logger);
                
                logger->WriteLog("Reduced state (stab):");
                reduced->log(logger);

                logger->WriteLog("Reduced state (density matrix):");
                mixedReduced->logMatrix(logger);   
                
                string reducedStabilizerFileName = outputDir + "/reduced/stabilizers.csv";
                string reducedPhaseFileName = outputDir + "/reduced/phase.csv";

                string reducedDensityMatrixFileName = outputDir + "/reduced/densityMatrix.csv";
                ofstream reducedDensityMatrixFile(reducedDensityMatrixFileName);

                string reducedIndicesFileName = outputDir + "/reduced/subset.csv";
                ofstream reducedIndicesFile(reducedIndicesFileName);

                vector<NoiseMap*> outputNoiseMapsReduced = reduced->getNoiseMaps();
                vector<tuple<string, string>> noiseOutputReducedFiles;

                int numNoiseMaps = outputNoiseMapsReduced.size();

                for(int i=0; i<numNoiseMaps; i++)
                {
                    string noiseOperatorFile = outputDir + "/reduced/noisemap"+to_string(i)+"-operators.csv";
                    string weightsFile = outputDir + "/reduced/noisemap"+to_string(i)+"-weights.csv";

                    noiseOutputReducedFiles.push_back(make_tuple(noiseOperatorFile, weightsFile));
                }

                stateStream.write(reduced, reducedStabilizerFileName, reducedPhaseFileName, noiseOutputReducedFiles, reduced->getDimension());
                stateStream.writeMatrixComplexFile(reducedDensityMatrixFile, mixedReduced);
                stateStream.writePhaseFile(reducedIndicesFile, new VectorInt(subset));
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
}

vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

NoisyState* readNoisyState(string inputFilesState, string inputFilesNoiseMaps, int dimension, Logger *logger)
{
        ifstream inputFilesStateStream(inputFilesState);
        vector<string> stabilizerFiles = parseInputStateFile(inputFilesStateStream);

        string stabilizerTableauFileName = stabilizerFiles[0];
        string phaseFileName = stabilizerFiles[1];

        logger->WriteLog("Stabilizer tableau file name: " + stabilizerTableauFileName);
        logger->WriteLog("Phase file name: " + phaseFileName);

        ifstream inputFilesNoiseMapsStream(inputFilesNoiseMaps);
        vector<vector<string>> noiseMapFiles = parseNoiseMapsFile(inputFilesNoiseMapsStream);

        vector<tuple<string, string>> noiseMaps;
        for(int i=0; i < noiseMapFiles.size(); i++)
        {
            if(logger)
            logger->WriteLog("Noise map #" + to_string(i) + ", operator file: " + noiseMapFiles[i][0]);
            logger->WriteLog("Noise map #" + to_string(i) + ", weights file: " + noiseMapFiles[i][1]);

            noiseMaps.push_back(make_tuple(noiseMapFiles[i][0], noiseMapFiles[i][1]));
        }

        CsvStateStream stateStream(nullptr);
        NoisyState* inputState = stateStream.read(stabilizerTableauFileName, 
            phaseFileName, 
            noiseMaps, 
        dimension);

        return inputState;
}

NoisyState* runCircuit(string inputFilesState, string inputFilesNoiseMaps, string circuitJson, Logger* logger, string logParameter)
{
        CircuitParserQubits parser(logger);
        Circuit *circuit=parser.Parse(circuitJson);

        NoisyState* inputState = readNoisyState(inputFilesState, inputFilesNoiseMaps, circuit->getDimension(), logger);
        StopWatch watch;
        watch.start();

        NoisyState* outputState = circuit->Apply(inputState);

        watch.stop();
        auto duration = watch.asMicroseconds();

        if(logParameter.compare("none") == 0)
        {
            cout << "Execution time: " + to_string(duration) + "[us]" << std::endl;
        }
        else if(logParameter.compare("file") == 0)
        {
            cout << "Execution time: " + to_string(duration) + "[us]" << std::endl;
        }

        logger->WriteLog("Execution time: " + to_string(duration) + "[us]");
        return outputState;
}

vector<string> parseTestsFile(ifstream &file)
{
    vector<string> files;
    for( std::string line; getline(file,line); )
        files.push_back(line);
    
    return files;
}

vector<string> parseInputStateFile(ifstream &file)
{
    //there should be only entry, and it contains
    //[0]: stabilizer tableau file name
    //[1]: phase file name
    string delim(",");
    vector<string> files;
    for( std::string line; getline(file,line); )
    {
        files=split(line, delim);
    }

    return files;
}

vector<vector<string>> parseNoiseMapsFile(ifstream &file)
{
    //file format of noise maps input file spec:
    //[i]: noise_operators filename, weights file name
    string delim(",");
    vector<vector<string>> files;
    for( std::string line; getline(file,line); )
    {
        vector<string> row=split(line, delim);
        files.push_back(row);
    }

    return files;
}


