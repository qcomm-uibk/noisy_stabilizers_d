/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#include "testReducedSeparableTableau.h"

#include "../../circuit-model/state.h"
#include <ctime>

#include "../../utils/matrix.h"
#include "../../utils/vectorint.h"
#include "../../utils/mathUtils.h"
#include "../../logging/loggerCout.h"
#include "../../logging/nullLogger.h"


TestReducedSeparableTableau::TestReducedSeparableTableau(Logger *logger)
{
    mLogger = logger;
}

TestReducedSeparableTableau::~TestReducedSeparableTableau()
{
}

Graph TestReducedSeparableTableau::generateRandomGraph(long nodes, double p)
{
    Graph graph(nodes);
    for (auto i = 0; i < nodes; i++)
    {
        for (auto j = i; j < nodes; j++)
        {
            std::srand(std::time({}));
            float prob = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            if (prob <= p)
            {
                graph.insert_edge(i,j);
            }
        }
    }
    return graph;
}

void TestReducedSeparableTableau::runTest()
{
    int n = 25;
    int p = 7;
    // G = nx.fast_gnp_random_graph(n, 0.33)
    // G = nx.fast_gnp_random_graph(n, 0.9)
    Graph G = generateRandomGraph(n, 0.9);
    State* state = new State(new MatrixInt(n, 2*n, 0), new VectorInt(), p);

    for (int i = 0; i < n; i++)
        state->getStabilizer()->setValue(i, i+n, 1);
    
    vector<tuple<int,int>> edges = G.getEdges();
    for(int i =0; i < edges.size(); i++)
    {
        // Random number between 1 and p
        std::srand(std::time({})); // use current time as seed for random generator
        // range = p - 1;
        int edge_weight = rand() % (p-1) + 1;
        
        state->getStabilizer()->setValue(get<0>(edges.at(i)), get<1>(edges.at(i)), edge_weight);
        state->getStabilizer()->setValue(get<1>(edges.at(i)), get<0>(edges.at(i)), edge_weight);
    }

    VectorInt* separableIndices = new VectorInt({2, 7, 12});
    for (int col = 0; col < separableIndices->getSize(); col++)
    {
        for (int row = 0; row < n; row++)
        {
            if (separableIndices->contains(row))
                continue;
            state->getStabilizer()->setValue(row, separableIndices->getValue(col), 0);
            state->getStabilizer()->setValue(row, separableIndices->getValue(col) + n, 0);
        }
    }

    for (int row = 0; row < separableIndices->getSize(); row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (separableIndices->contains(col))
                continue;
            state->getStabilizer()->setValue(separableIndices->getValue(row), col, 0);
            state->getStabilizer()->setValue(separableIndices->getValue(row), col + n, 0);
        }
    }

    // Set random phases and interchange some X/Y/Z
    for (int col = 0; col < n; col++)
    {
        std::srand(std::time({})); // use current time as seed for random generator
        int intCase = rand() % 4;
        // Random number between 1 and p
        int range = p - 1;
        int param = rand() % range + 1;
        VectorInt* tmp;
        int param_inverse;

        switch (intCase)
        {
        case 0:
            continue;
        case 1:        // apply S (param)
            for (int indexRow = 0; indexRow < state->getStabilizer()->getNumRows(); indexRow++)
                state->getStabilizer()->setValue(indexRow, col, state->getStabilizer()->getValue(indexRow, col) + param * state->getStabilizer()->getValue(indexRow, col+n));
            break;
        case 2:     // Apply M(factor)
            param_inverse = MathUtils::mod(MathUtils::power(param, p-2), p);
            for (int indexRow = 0; indexRow < state->getStabilizer()->getNumRows(); indexRow++)
            {
                state->getStabilizer()->setValue(indexRow, col, state->getStabilizer()->getValue(indexRow, col) * param_inverse);
                state->getStabilizer()->setValue(indexRow, col + n, state->getStabilizer()->getValue(indexRow, col + n) * param_inverse);
            }
            break;
        case 3:  // Apply H
            tmp = state->getStabilizer()->getFullColumn(col);
            for (int indexRow = 0; indexRow < state->getStabilizer()->getNumRows(); indexRow++)
            {
                state->getStabilizer()->setValue(indexRow, col, state->getStabilizer()->getValue(indexRow, col + n));
                state->getStabilizer()->setValue(indexRow, col + n, -tmp->getValue(indexRow));
            }
            break;
        default:
            break;
        }
    }
    state->getStabilizer()->mod(p);
    
    std::srand(std::time({})); // use current time as seed for random generator

    for (int i = 0; i < n; i++)
    {
        state->getPhase()->pushBack(rand() % (p+1));
    }

    State* stateInitial = state->copy();

    // Confuse the situation by adding rows together arbitrarily 
    for (int k = 0; k < MathUtils::power(n, 2); k++)
    {
        std::srand(std::time({})); // use current time as seed for random generator
        int i = rand() % n;
        int j = rand() % n;
        
        if (i == j) continue;
        state = state->addRows(i,j);
    }

    State* stateOut = state->XtoRrefMod();
    stateInitial = stateInitial->XtoRrefMod();
    bool isOk = stateInitial->isEqual(stateOut);
    string result = "YES";
    if(!isOk)
        result = "NO!";

    mLogger->WriteLog("stateInitial == stateOut?");
    mLogger->WriteLog(result);

    // TODO: I have not added this line
    // print(np.allclose(np.sort(np.hstack([T_out, s_out.reshape(-1, 1)]), axis = 0),
    // np.sort(np.hstack([T_initial, s_initial.reshape(-1,1)]), axis = 0)))

    // JW: these ones are just to check if there was a mistake when specifying the test in itself.
    // State* stateSmall = state->reduceSeparableTableau(separableIndices);
    // stateSmall->getStabilizer()->logMatrix(mLogger);
    // stateSmall->getPhase()->logVector(mLogger);

    // State* tempStateSmall = stateSmall->XtoRrefMod();
    // tempStateSmall->getStabilizer()->logMatrix(mLogger);
    // tempStateSmall->getPhase()->logVector(mLogger);
}